#include "ImagePacket.h"
#include <cmath>
#include <array>
namespace TheosRenderPipeline::NeuralRendering {
namespace {
using Microsoft::WRL::ComPtr;
std::unexpected<Error> Invalid(const char* message){return std::unexpected(Error{ErrorKind::InvalidInput,0,message});}
bool SameObject(IUnknown* a,IUnknown* b){
    if(!a||!b)return false;ComPtr<IUnknown> left,right;
    return SUCCEEDED(a->QueryInterface(IID_PPV_ARGS(&left)))&&SUCCEEDED(b->QueryInterface(IID_PPV_ARGS(&right)))&&left.Get()==right.Get();
}
template<class T>bool OnDevice(T* child,ID3D12Device* expected){
    ComPtr<ID3D12Device> actual;
    return child && SUCCEEDED(child->GetDevice(IID_PPV_ARGS(&actual))) && SameObject(actual.Get(),expected);
}
bool Texture(ID3D12Resource* r,ImageExtent e,DXGI_FORMAT format,bool uav=false){
    if(!r||!e.width||!e.height)return false;const auto d=r->GetDesc();
    return d.Dimension==D3D12_RESOURCE_DIMENSION_TEXTURE2D&&d.Width==e.width&&d.Height==e.height&&
        d.DepthOrArraySize==1&&d.MipLevels==1&&d.SampleDesc.Count==1&&d.SampleDesc.Quality==0&&d.Format==format&&
        (!uav||(d.Flags&D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS));
}
}
Result<void> ValidateImagePacket(ID3D12GraphicsCommandList* list,const ImagePacket& p,const StageContract& c){
    if(!c.device || !c.queue || !c.adapterLuid.Valid())return Invalid("NR stage device/queue identity missing");
    const auto luid=c.device->GetAdapterLuid();
    if(luid.LowPart!=c.adapterLuid.low || luid.HighPart!=c.adapterLuid.high)
        return std::unexpected(Error{ErrorKind::IdentityMismatch,0,"NR stage actual device LUID differs"});
    if(FAILED(c.device->GetDeviceRemovedReason()))return std::unexpected(Error{ErrorKind::Runtime,0,"NR stage device removed"});
    if(!OnDevice(c.queue.Get(),c.device.Get()) || !OnDevice(list,c.device.Get()) ||
        c.queue->GetDesc().Type!=D3D12_COMMAND_LIST_TYPE_DIRECT || list->GetType()!=D3D12_COMMAND_LIST_TYPE_DIRECT)
        return Invalid("NR recording list/queue is missing or belongs to another device/type");
    if(!p.epoch || !p.batchId || !p.imageId || !p.sourceId || p.previousSourceId>=p.sourceId ||
        p.guideEpoch!=p.epoch || p.guideSourceId!=p.sourceId || !std::isfinite(p.presentationTime))
        return Invalid("NR image/source/guide epoch identity is invalid or stale");
    if(p.kind==ImageKind::Generated)
        return std::unexpected(Error{ErrorKind::Unsupported,0,"NR generated guide/output recipe is not yet qualified"});
    if(p.kind!=ImageKind::Real || p.guideOrigin!=GuideOrigin::RealSource ||
        !p.interpolationFraction || *p.interpolationFraction!=1)
        return Invalid("NR real-source guide provenance/time is invalid");
    if(p.colorDomain!=ColorDomain::Linear)return Invalid("NR color domain is unknown/unconverted");
    if(p.colorExtent!=c.colorExtent || p.guideExtent!=c.guideExtent)
        return Invalid("NR packet extent differs from retained stage contract");
    const std::array resources{p.color.Get(),p.output.Get(),p.depth.Get(),p.motion.Get()};
    for(size_t i=0;i<resources.size();++i){
        if(!OnDevice(resources[i],c.device.Get()))return Invalid("NR resource missing or foreign device");
        for(size_t j=0;j<i;++j)if(SameObject(resources[i],resources[j]))return Invalid("NR input/output/guide resource alias");
        if(p.ui && SameObject(resources[i],p.ui.Get()))return Invalid("NR world/guide resource aliases dedicated UI");
    }
    if(p.ui && !OnDevice(p.ui.Get(),c.device.Get()))return Invalid("NR dedicated UI belongs to another device");
    if(!Texture(p.color.Get(),p.colorExtent,DXGI_FORMAT_R16G16B16A16_FLOAT) ||
        !Texture(p.output.Get(),p.colorExtent,DXGI_FORMAT_R16G16B16A16_FLOAT,true) ||
        !Texture(p.depth.Get(),p.guideExtent,DXGI_FORMAT_R32_FLOAT) ||
        !Texture(p.motion.Get(),p.guideExtent,DXGI_FORMAT_R16G16_FLOAT))return Invalid("NR texture format/shape/flags invalid");
    if(!std::isfinite(p.motionScaleX) || !std::isfinite(p.motionScaleY) || !p.motionScaleX || !p.motionScaleY)
        return Invalid("NR motion units/scaling invalid");
    if(p.colorState!=D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE ||
        p.depthState!=D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE ||
        p.motionState!=D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE ||
        p.outputState!=D3D12_RESOURCE_STATE_UNORDERED_ACCESS)return Invalid("NR declared evaluation states invalid");
    if(bool(p.producerFence)!=bool(p.producerFenceValue))return Invalid("NR producer fence/value incomplete");
    if(p.producerFence){
        const auto completed=p.producerFence->GetCompletedValue();
        if(!OnDevice(p.producerFence.Get(),c.device.Get()) || completed==UINT64_MAX || completed<p.producerFenceValue)
            return std::unexpected(Error{ErrorKind::Retirement,0,"NR producer ownership has not retired"});
    }
    return {};
}
}
