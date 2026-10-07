#include "Upscaling/FSRHostResources.h"
#include "Upscaling/FSRGenerationGuideAdapter.h"
#include "InteropTestRig.h"
#include <filesystem>
using namespace TheosRenderPipeline;
using namespace TheosRenderPipeline::Upscaling;
using namespace InteropFixture;

static void GenerationOnlyRuntime(const std::filesystem::path& root)
{
    FsrRuntime runtime;
    Require(bool(runtime.LoadGenerationOnly(root)), "generation-only runtime loads without SR ownership");
    Require(!GetModuleHandleW(L"amd_fidelityfx_upscaler_dx12.dll"), "ExternalSourceCreatesNoSrContext: SR module was not loaded");
    Require(runtime.Profile()==FsrRuntimeProfile::Official, "InactiveMlSrDoesNotSelectInt8: FG uses official runtime");
    Require(!runtime.Enumerate(reinterpret_cast<ID3D12Device*>(1)), "generation-only runtime rejects SR queries");
    Require(!runtime.EnumerateForEffect(reinterpret_cast<ID3D12Device*>(1),FsrEffect::Upscale), "generation-only runtime also rejects effect-tagged SR queries");
    Require(!runtime.LoadGenerationOnly(root), "generation-only owner cannot replace a live loader");
}
static void Guides(const std::filesystem::path& root)
{
    Rig rig;
    FsrHostResources resources(root);
    FsrInputPolicy policy{true,false,true,true};
    Require(bool(resources.PrepareExternalSizing(rig.device11.Get(),{8,4},{16,8},DXGI_FORMAT_R8G8B8A8_UNORM,ColorEncoding::Gamma22,policy)), "external sizing uses actual producer adapter");
    Require(resources.ExternalSource() && !resources.Upscaler() && !resources.ContextOwned(), "external mode has no SR owner");
    Require(bool(resources.CompleteExternalStartup()) && resources.GenerationInputsReady(), "shared generation guides allocate without an SR context");
    Require(resources.GenerationInputPolicy()==policy && resources.RenderExtent()==Extent{8,4}, "external guide convention and extent retained");
    ComPtr<ID3D11Texture2D> depth,motion;
    D3D11_TEXTURE2D_DESC desc{};resources.Depth11()->GetDesc(&desc);
    Check(rig.device11->CreateTexture2D(&desc,nullptr,&depth),"producer depth");
    resources.Motion11()->GetDesc(&desc);Check(rig.device11->CreateTexture2D(&desc,nullptr,&motion),"producer motion");
    float depths[32];for(auto& v:depths)v=0.375f;
    uint16_t motions[64];for(unsigned i=0;i<64;++i)motions[i]=i%2?0xbc00:0x3800; // -1, +0.5 half-float
    rig.context11->UpdateSubresource(depth.Get(),0,nullptr,depths,8*sizeof(float),0);
    rig.context11->UpdateSubresource(motion.Get(),0,nullptr,motions,8*4,0);
    FsrGenerationGuideAdapter adapter(resources.Bridge(),resources.Resources(),resources.Depth11(),resources.Motion11());
    UpscaleFrame frame{};frame.sourceId=1;frame.sourceEpoch=3;frame.render=frame.subrect={8,4};frame.display={16,8};
    frame.depth=depth.Get();frame.motion=motion.Get();frame.jitterX=0.25f;frame.jitterY=-0.375f;
    frame.motionConvention={8,4,true,true};frame.camera.depthInverted=true;
    Require(bool(adapter.Prepare(frame)), "valid current external guides copied");
    frame.sourceId=2;Require(bool(adapter.Prepare(frame)),"next guide producer does not require a fictitious SR dispatch");
    Check(resources.Bridge()->Drain(),"external producer/consumer retirement");
    auto read=[&](ID3D11Texture2D* texture,const void* expected,size_t pixelBytes){
        D3D11_TEXTURE2D_DESC staging{};texture->GetDesc(&staging);staging.BindFlags=0;staging.MiscFlags=0;staging.Usage=D3D11_USAGE_STAGING;staging.CPUAccessFlags=D3D11_CPU_ACCESS_READ;
        ComPtr<ID3D11Texture2D> copy;Check(rig.device11->CreateTexture2D(&staging,nullptr,&copy),"guide readback resource");
        rig.context11->CopyResource(copy.Get(),texture);D3D11_MAPPED_SUBRESOURCE mapped{};Check(rig.context11->Map(copy.Get(),0,D3D11_MAP_READ,0,&mapped),"guide readback");
        for(unsigned y=0;y<4;++y)Require(!std::memcmp(static_cast<char*>(mapped.pData)+mapped.RowPitch*y,static_cast<const char*>(expected)+8*pixelBytes*y,8*pixelBytes),"actual shared guide pixels retained");
        rig.context11->Unmap(copy.Get(),0);
    };
    read(resources.Depth11(),depths,4);read(resources.Motion11(),motions,4);
    Require(frame.jitterX==0.25f && frame.jitterY==-0.375f && frame.motionConvention.scaleX==8, "ExternalGuidesPreserveJitterAndMotionConvention: caller snapshot immutable");
    Require(!adapter.Prepare(frame), "GuidesRequireCurrentMatchingSource: duplicate identity rejected");
    frame.sourceId=3;frame.depth=nullptr;Require(!adapter.Prepare(frame),"missing depth rejects current source");
    frame.depth=depth.Get();frame.render.width=9;Require(!adapter.Prepare(frame),"mismatched render extent rejected");frame.render.width=8;
    ComPtr<ID3D11Device> foreign;ComPtr<ID3D11DeviceContext> foreignContext;
    Check(D3D11CreateDevice(nullptr,D3D_DRIVER_TYPE_WARP,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&foreign,nullptr,&foreignContext),"foreign guide device");
    depth->GetDesc(&desc);ComPtr<ID3D11Texture2D> foreignDepth;Check(foreign->CreateTexture2D(&desc,nullptr,&foreignDepth),"foreign guide texture");
    frame.depth=foreignDepth.Get();Require(!adapter.Prepare(frame),"foreign producer guide rejected");
    frame.depth=depth.Get();Require(bool(adapter.Prepare(frame)),"invalid input does not consume a valid source identity");
    Require(bool(resources.ReleaseSizedAfterRetirement()) && !resources.GenerationInputsReady(),"external guides released only after retirement");
    Require(bool(resources.ResizeExternalSizingAfterRetirement({4,2},{8,4},DXGI_FORMAT_R8G8B8A8_UNORM)) && bool(resources.CompleteExternalStartup()),"external resize retains official runtime and creates no SR context");
    Require(!resources.Upscaler() && !GetModuleHandleW(L"amd_fidelityfx_upscaler_dx12.dll"),"no hidden second upscaler after resize");
    Require(bool(resources.Retire()),"external owner fully retired");rig.ValidateDebug();
}
int main(int argc,char** argv)
{
    Require(argc>=2,"runtime root required");const auto root=std::filesystem::absolute(argv[1]);
    if(argc==3 && std::string_view(argv[2])=="--gpu")Guides(root);else GenerationOnlyRuntime(root);
    std::puts("PASS: generation-only official runtime and external guide ownership");
}
