#include "fsr-fg/PresentationTestRig.h"
#include <future>
#include <thread>
using namespace PresentationFixture;
int main(int argc,char** argv)
{
    Require(argc==2,"fixture root");PresentationFixture::Rig rig(argv[1]);FsrPresentation p;rig.Create(p);Require(bool(p.CompleteStartup(rig.limits,rig.fgProvider)),"complete startup");
    for(int i=0;i<16;++i)Check(rig.Source(p),"warm source");
    // A vendor-controlled Present pauses while holding the production SDK lock.
    // A second presenter operation must wait, and the inherited callback must
    // complete without ever acquiring that lock again.
    auto submit=[&](const UpscaleFrame& frame,bool requested){return p.Present(frame,UpscaleOutcome::Temporal,rig.guides,p.SceneTarget11(),ColorEncoding::SRGB,rig.ui.Get(),nullptr,true,false,requested,0,0);};
    for(int action=0;action<3;++action){rig.mode(22);const auto first=rig.frame;auto next=first;++next.sourceId;
        auto present=std::async(std::launch::async,[&,first]{return submit(first,true);});
        const auto deadline=std::chrono::steady_clock::now()+std::chrono::seconds(2);
        while(!rig.stat(12) && std::chrono::steady_clock::now()<deadline)std::this_thread::yield();Require(rig.stat(12)!=0,"fixture Present entered");
        auto operation=std::async(std::launch::async,[&,next,action]()->bool{if(action==2)return bool(p.Retire());return SUCCEEDED(submit(next,action==0));});
        Require(operation.wait_for(std::chrono::milliseconds(30))==std::future_status::timeout,action==0?"PresentVsPrepareSerialized":action==1?"PresentVsDisableSerialized":"PresentVsDestroySerialized");
        Check(present.get(),"CallbackNeverRelocks");Require(operation.get(),"serialized operation progresses");rig.frame.sourceId+=2;rig.mode(0);
    }
    // Recreate one owner at a time on the same HWND, including new dimensions.
    auto changed=rig.desc;changed.BufferDesc.Width=160;changed.BufferDesc.Height=96;
    FsrPresentation resized;Require(bool(resized.Create(rig.factory.Get(),rig.runtime,rig.bridge,changed,rig.swapchainProvider)),"same HWND dimension recreation");
    auto limits=rig.limits;limits.display={160,96};Require(bool(resized.CompleteStartup(limits,rig.fgProvider)),"new fixed extent context");
    auto* retained=resized.SceneTarget11();auto* chain=resized.SwapChain();rig.mode(23);
    Require(!resized.BeforeResize() && resized.SceneTarget11()==retained && resized.SwapChain()==chain,"FailedResizeKeepsOwnedResources");
    Require(rig.stat(8)>0 && rig.stat(10)==0,"UiUnregisteredBeforeRelease");rig.mode(0);Require(bool(resized.Retire()),"failed SDK wait can retry real retirement");
    rig.ValidateDebug();std::puts("PASS: session serialized Present/callback/lifecycle and failed retirement owner preservation (vendor double)");
}
