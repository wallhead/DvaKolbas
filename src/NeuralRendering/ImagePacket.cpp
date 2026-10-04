#include "ImagePacket.h"
#include <cmath>
#include <array>
#include <sstream>
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
Result<void> FenceDeviceIdentity::Initialize(ID3D12Device* device){
    if(!device||reference_)return Invalid("NR fence identity initialization missing/already attempted");
    ComPtr<ID3D12Device> native;
    auto hr=device->QueryInterface(IID_PPV_ARGS(&host_));
    if(SUCCEEDED(hr))hr=device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&reference_));
    if(SUCCEEDED(hr))hr=reference_->GetDevice(IID_PPV_ARGS(&native));
    if(SUCCEEDED(hr))hr=native.As(&native_);
    if(FAILED(hr))return std::unexpected(Error{ErrorKind::IdentityMismatch,hr,"NR private fence device identity capture failed"});
    return {};
}
Result<void> FenceDeviceIdentity::Validate(ID3D12Fence* fence,ID3D12Device* expectedHost)const{
    if(!host_||!native_||!fence||!expectedHost)return Invalid("NR fence identity not initialized or fence/host missing");
    if(!SameObject(host_.Get(),expectedHost))return std::unexpected(Error{ErrorKind::IdentityMismatch,0,"NR fence anchor belongs to a different host device"});
    ComPtr<ID3D12Device> device;ComPtr<IUnknown> actual;
    auto hr=fence->GetDevice(IID_PPV_ARGS(&device));
    if(SUCCEEDED(hr))hr=device.As(&actual);
    if(FAILED(hr))return std::unexpected(Error{ErrorKind::IdentityMismatch,hr,"NR input fence device query failed"});
    if(actual!=host_&&actual!=native_){
        std::ostringstream message;message<<"NR foreign fence device: host="<<host_.Get()<<" referenceOwner="<<native_.Get()<<" inputOwner="<<actual.Get();
        return std::unexpected(Error{ErrorKind::IdentityMismatch,0,message.str()});
    }
    return {};
}
static Result<void> ValidatePacket(ID3D12GraphicsCommandList* list,const ImagePacket& p,const StageContract& c,const FenceDeviceIdentity* fences,bool pending){
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
    const auto colorFormat=NrColorFormat(p.colorDomain);
    if(colorFormat==DXGI_FORMAT_UNKNOWN)return Invalid("NR color domain is unknown/unqualified");
    if(p.colorExtent!=c.colorExtent || p.guideExtent!=c.guideExtent)
        return Invalid("NR packet extent differs from retained stage contract");
    const std::array resources{p.color.Get(),p.output.Get(),p.depth.Get(),p.motion.Get()};
    for(size_t i=0;i<resources.size();++i){
        if(!OnDevice(resources[i],c.device.Get()))return Invalid("NR resource missing or foreign device");
        for(size_t j=0;j<i;++j)if(SameObject(resources[i],resources[j]))return Invalid("NR input/output/guide resource alias");
        if(p.ui && SameObject(resources[i],p.ui.Get()))return Invalid("NR world/guide resource aliases dedicated UI");
    }
    if(p.ui && !OnDevice(p.ui.Get(),c.device.Get()))return Invalid("NR dedicated UI belongs to another device");
    if(!Texture(p.color.Get(),p.colorExtent,colorFormat) ||
        !Texture(p.output.Get(),p.colorExtent,colorFormat,true) ||
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
        if(fences){auto identity=fences->Validate(p.producerFence.Get(),c.device.Get());if(!identity)return identity;}
        else if(!OnDevice(p.producerFence.Get(),c.device.Get()))return std::unexpected(Error{ErrorKind::IdentityMismatch,0,"NR producer fence belongs to a different device"});
        const auto completed=p.producerFence->GetCompletedValue();
        if(completed==UINT64_MAX)return std::unexpected(Error{ErrorKind::Retirement,0,"NR producer fence reports device removal"});
        if(!pending&&completed<p.producerFenceValue){
            std::ostringstream message;message<<"NR producer ownership has not retired: target="<<p.producerFenceValue<<" completed="<<completed;
            return std::unexpected(Error{ErrorKind::Retirement,0,message.str()});
        }
    }
    return {};
}
Result<void> ValidateImagePacket(ID3D12GraphicsCommandList* list,const ImagePacket& p,const StageContract& c,const FenceDeviceIdentity* fences){
    return ValidatePacket(list,p,c,fences,false);
}
Result<void> QueuedImageAdmission::Validate(ID3D12GraphicsCommandList* list,const ImagePacket& p,const StageContract& c,const FenceDeviceIdentity& fences){
    return ValidatePacket(list,p,c,&fences,true);
}
}
