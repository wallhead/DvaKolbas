#include "FrameGen/XessGenerationCompletedSource.h"
#include "FrameGen/XessGenerationPolicy.h"
#include "FrameGen/SourceFrameEvaluator.h"
#include "XessFgFrameFixture.h"
#include <wrl/client.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
using namespace TheosRenderPipeline;
using namespace TheosRenderPipeline::Upscaling;
using Microsoft::WRL::ComPtr;
static void Require(bool value,const char* reason){if(!value){std::fprintf(stderr,"FAIL: %s\n",reason);std::exit(1);}}
struct RouteOperations {
    unsigned placement{},passes{};ID3D11Texture2D* scene{};
    UpscaleOutcome outcome{UpscaleOutcome::Temporal};
    std::vector<std::string> events;
    void CopyInput(ID3D11DeviceContext*,const UpscaleFrame&){events.emplace_back("input");}
    bool EvaluateOptionalPreUpscale(UpscaleFrame&) {
        if(placement==1)for(unsigned i=0;i<passes;++i)events.emplace_back("NR-before");return true;
    }
    void RenderReShade(const UpscaleFrame&,bool){}
    Result<UpscaleOutcome> EvaluateUpscaler(UpscaleFrame& frame){events.emplace_back("SR");frame.output=scene;return outcome;}
    void UpscaleSucceeded(){}
    bool EvaluateOptionalPostUpscale(UpscaleFrame&,UpscaleOutcome result) {
        if(result==UpscaleOutcome::Temporal && placement==2)for(unsigned i=0;i<passes;++i)events.emplace_back("NR-after");return true;
    }
    GenerationPreparationStatus PrepareGeneration(const UpscaleFrame& frame) {
        Require(frame.output==scene,"FG sees completed real SR/NR output");events.emplace_back("FG");
        return GenerationPreparationStatus::Succeeded;
    }
};
static ComPtr<ID3D11Texture2D> Texture(ID3D11Device* device,UINT size,DXGI_FORMAT format)
{
    D3D11_TEXTURE2D_DESC d{};d.Width=d.Height=size;d.Format=format;d.MipLevels=d.ArraySize=d.SampleDesc.Count=1;
    d.BindFlags=D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET;
    ComPtr<ID3D11Texture2D> result;Require(SUCCEEDED(device->CreateTexture2D(&d,nullptr,&result)),"real source texture");return result;
}
int main()
{
    ComPtr<ID3D11Device> device;ComPtr<ID3D11DeviceContext> context;
    Require(SUCCEEDED(D3D11CreateDevice(nullptr,D3D_DRIVER_TYPE_WARP,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&device,nullptr,&context)),"WARP producer");
    auto scene=Texture(device.Get(),4,DXGI_FORMAT_R8G8B8A8_UNORM);
    for(auto backend:{BackendKind::Dlaa,BackendKind::Dlss,BackendKind::Fsr,BackendKind::Xess})
    for(UINT render:{2u,4u})for(unsigned placement:{0u,1u,2u})for(unsigned passes:{1u,2u,3u}) {
        auto depth=Texture(device.Get(),render,DXGI_FORMAT_R32_FLOAT),motion=Texture(device.Get(),render,DXGI_FORMAT_R16G16_FLOAT);
        auto metadata=XessFgFrame();metadata.backend=backend;metadata.render=metadata.subrect={render,render};metadata.display={4,4};
        metadata.depth=depth.Get();metadata.motion=motion.Get();metadata.camera.projection[0]=1;
        metadata.motionConvention={float(render),float(render),true,false};metadata.sourceEpoch=1+placement*3+passes;
        RouteOperations route;route.placement=placement;route.passes=passes;route.scene=scene.Get();
        auto routed=SourceFrameEvaluator::Evaluate(context.Get(),metadata,route);
        Require(routed.outcome==UpscaleOutcome::Temporal,"temporal source routed");
        std::vector<std::string> expected{"input"};
        if(placement==1)for(unsigned i=0;i<passes;++i)expected.emplace_back("NR-before");
        expected.emplace_back("SR");
        if(placement==2)for(unsigned i=0;i<passes;++i)expected.emplace_back("NR-after");
        expected.emplace_back("FG");
        Require(route.events==expected,"production source evaluator orders NR around SR, then FG exactly once");
        for(auto outcome:{UpscaleOutcome::RepeatedOutput,UpscaleOutcome::SpatialRecovery}) {
            auto suppressed=route;suppressed.events.clear();suppressed.outcome=outcome;
            auto result=SourceFrameEvaluator::Evaluate(context.Get(),metadata,suppressed);
            Require(result.preparation==GenerationPreparationStatus::NotRequested,"repeat/recovery does not prepare FG");
            for(const auto& event:suppressed.events)Require(event!="FG" && event!="NR-after","repeat/recovery does not evaluate After NR or FG");
        }
        // Final scene publication must not reconstruct or renumber the source.
        const auto completed=CompleteXessGenerationSource(metadata,scene.Get(),ColorEncoding::Gamma22);
        Require(bool(completed),"all SR/NR metadata routes accept the final real scene");
        Require(completed->output==scene.Get() && completed->sourceId==metadata.sourceId && completed->sourceEpoch==metadata.sourceEpoch,"final scene/real source epoch preserved");
        Require(completed->depth==depth.Get() && completed->motion==motion.Get() && completed->depthExtent==metadata.render && completed->motionExtent==metadata.render,"original measured producer guides retained");
        Require(completed->outputEncoding==ColorEncoding::Gamma22 && completed->uiEncoding==ColorEncoding::SRGB,"scene and HUD encodings are explicit");
        XessGenerationHistory history;
        auto warm=history.Decide(*completed,UpscaleOutcome::Temporal,true,true,false);Require(warm.tag && !warm.generate && warm.reset,"source starts with a reset");
        history.Accept(completed->sourceId,completed->sourceEpoch);
        auto next=*completed;++next.sourceId;
        auto active=history.Decide(next,UpscaleOutcome::Temporal,true,true,false);Require(active.tag && active.generate,"unchanged real temporal source may generate");
        history.Accept(next.sourceId,next.sourceEpoch);
        ++next.sourceId;++next.sourceEpoch;
        auto changed=history.Decide(next,UpscaleOutcome::Temporal,true,true,false);Require(changed.reset && !changed.generate,"NR style/pass epoch changes reset downstream history");
        Require(!history.Decide(next,UpscaleOutcome::RepeatedOutput,true,true,false).tag,"repeat cannot run fresh NR/FG");
        Require(!history.Decide(next,UpscaleOutcome::SpatialRecovery,true,true,false).tag,"spatial recovery cannot interpolate");
        Require(!history.Decide(next,UpscaleOutcome::Temporal,false,true,false).tag,"FG off leaves source/NR publication usable");
    }
    auto invalid=XessFgFrame();invalid.display={4,4};
    Require(!CompleteXessGenerationSource(invalid,scene.Get(),ColorEncoding::Unknown),"unknown final encoding rejected");
    std::puts("PASS: final real SR/NR scene, measured guides, explicit HUD encoding and downstream epoch/reset admission");
}
