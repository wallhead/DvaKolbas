#include "GameSwapChain.h"
#include "NativeSwapChain.h"
#include <cstdio>
#include <wrl/client.h>

NvidiaHost* preparationHost{};
int failures{};
void Check(bool condition,const char* message)
{
    if(!condition){std::printf("FAIL %s\n",message);++failures;}
}

struct Fixture
{
    NvidiaHost host;
    NativeSwapChain native;
    GameSwapChain* wrapper;
    Fixture(bool fsr=false,bool extended=true,bool hasInner=true)
    {
        host.fsrFg=fsr;native.host=&host;native.interfaces=extended;
        wrapper=new GameSwapChain(hasInner?&native:nullptr,&host);
        preparationHost=&host;
    }
    ~Fixture()
    {
        wrapper->Release();preparationHost=nullptr;
        Check(native.references==1,"wrapper releases all native interface references");
    }
    HRESULT Present(bool present1,UINT flags=0,const DXGI_PRESENT_PARAMETERS* parameters=nullptr)
    {
        return present1?wrapper->Present1(2,flags,parameters):wrapper->Present(2,flags);
    }
};

int main()
{
    {
        // Real D3D11 objects at the facade boundary. The production outer
        // wrapper must route Intel device/buffer queries to the producer.
        using Microsoft::WRL::ComPtr;
        ComPtr<ID3D11Device> device;ComPtr<ID3D11DeviceContext> context;
        Check(SUCCEEDED(D3D11CreateDevice(nullptr,D3D_DRIVER_TYPE_WARP,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&device,nullptr,&context)),"retained D3D11 producer fixture");
        D3D11_TEXTURE2D_DESC desc{};desc.Width=4;desc.Height=3;desc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.MipLevels=desc.ArraySize=desc.SampleDesc.Count=1;desc.BindFlags=D3D11_BIND_RENDER_TARGET;
        ComPtr<ID3D11Texture2D> buffer;
        Check(device && SUCCEEDED(device->CreateTexture2D(&desc,nullptr,&buffer)),"render-sized game buffer fixture");
        Fixture f;f.host.xessFg=true;f.host.producer=device.Get();f.host.buffer=buffer.Get();
        ComPtr<ID3D11Device> queried;
        Check(SUCCEEDED(f.wrapper->GetDevice(IID_PPV_ARGS(&queried))) && queried.Get()==device.Get(),"Intel outer exposes retained D3D11 producer");
        for(UINT index:{0u,1u}) {
            ComPtr<ID3D11Texture2D> returned;
            Check(SUCCEEDED(f.wrapper->GetBuffer(index,IID_PPV_ARGS(&returned))) && returned.Get()==buffer.Get(),"Intel outer always exposes the stable render-sized D3D11 buffer");
            D3D11_TEXTURE2D_DESC actual{};if(returned)returned->GetDesc(&actual);
            Check(actual.Width==4 && actual.Height==3,"outer buffer retains source render extent");
        }
    }
    for(int backend:{0,1,2})for(bool present1:{false,true})
    {
        const bool fsr=backend==1;
        {
            Fixture f(fsr);f.host.xessFg=backend==2;f.host.prepareFailure=E_ABORT;
            Check(f.Present(present1)==E_ABORT,"failure raised during preparation reaches caller");
            Check(f.host.events==std::vector<int>{1},"failed preparation blocks native submission and completion");
        }
        {
            Fixture f(fsr);f.host.xessFg=backend==2;f.host.failure=DXGI_ERROR_DEVICE_REMOVED;
            Check(f.Present(present1)==DXGI_ERROR_DEVICE_REMOVED,"preexisting host failure reaches caller");
            Check(f.host.events.empty(),"terminal host does not prepare, submit or complete");
        }
        {
            Fixture f(fsr);f.host.xessFg=backend==2;DXGI_PRESENT_PARAMETERS parameters{};
            Check(f.Present(present1,DXGI_PRESENT_DO_NOT_WAIT,&parameters)==S_OK,"normal present succeeds");
            Check(f.host.events==std::vector<int>({1,2,3}),"prepare precedes native submit and completion");
            Check(f.host.sync==2&&f.host.flags==DXGI_PRESENT_DO_NOT_WAIT,"caller sync and flags forwarded");
            if(present1&&backend==0)Check(f.native.parameters==&parameters,"native Present1 parameters forwarded");
        }
        {
            Fixture f(fsr);f.host.xessFg=backend==2;f.host.submitResult=DXGI_ERROR_DEVICE_RESET;
            Check(f.Present(present1)==DXGI_ERROR_DEVICE_RESET,"native failure reaches caller");
            Check(f.host.events==std::vector<int>({1,2,3})&&f.host.completedResult==DXGI_ERROR_DEVICE_RESET,
                "submitted native failure reaches completion exactly once");
        }
        {
            Fixture f(fsr);f.host.xessFg=backend==2;f.host.prepareFailure=E_ABORT;
            Check(f.Present(present1,DXGI_PRESENT_TEST)==S_OK,"TEST reaches native boundary without preparation");
            Check(f.host.events==std::vector<int>{2},"TEST does not produce or complete a source frame");
        }
    }
    {
        Fixture f(false,false);
        Check(f.Present(true)==E_NOINTERFACE&&f.host.events.empty(),"missing native Present1 interface blocks hooks");
    }
    {
        Fixture f(false,true,false);
        Check(f.Present(false)==E_UNEXPECTED&&f.host.events.empty(),"missing native swapchain blocks hooks");
    }
    {
        Fixture f(true);DXGI_PRESENT_PARAMETERS parameters{};parameters.DirtyRectsCount=1;
        Check(f.Present(true,0,&parameters)==E_INVALIDARG&&f.host.events.empty(),"FSR partial updates refused before production");
    }
    for(bool present1:{false,true})
    {
        Fixture f(true);f.host.suspended=true;
        Check(f.Present(present1)==DXGI_STATUS_OCCLUDED&&f.host.events.empty(),"FSR suspended present does not consume a source");
    }
    std::printf("Game-facing production Present/Present1: %d failures (explicit host/native facades)\n",failures);
    return failures?1:0;
}
