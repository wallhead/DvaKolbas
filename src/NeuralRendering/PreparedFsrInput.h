#pragma once
#include "Error.h"
#include <d3d11_4.h>
#include <d3d12.h>
#include <wrl/client.h>
#include <functional>
#include <memory>
#include <utility>
namespace TheosRenderPipeline::NeuralRendering {
class PreparedBeforeUpscale;
// One move-only reader lease. The producer retains all resources independently
// until a genuine downstream reader retires. Dropping an unconsumed lease faults
// that producer instead of making its image reusable.
class PreparedFsrInput {
public:
    PreparedFsrInput()=default;
    PreparedFsrInput(const PreparedFsrInput&)=delete;
    PreparedFsrInput& operator=(const PreparedFsrInput&)=delete;
    PreparedFsrInput(PreparedFsrInput&& other)noexcept{Swap(other);}
    PreparedFsrInput& operator=(PreparedFsrInput&& other)noexcept{if(this!=&other){PreparedFsrInput old;Swap(old);Swap(other);}return *this;}
    ~PreparedFsrInput(){if(abandon_)abandon_();}
    bool Valid()const noexcept{return bool(track_);}
    uint64_t SourceId()const noexcept{return source_;}
    uint64_t Epoch()const noexcept{return epoch_;}
    bool Reset()const noexcept{return reset_;}
    ID3D11DeviceContext* Context()const noexcept{return context_.Get();}
    // Successful TrackReader clears all public resources and frame identity.
    // The producer independently retains them through the genuine reader.
    ID3D11Texture2D* Color()const noexcept{return color_.Get();}
    ID3D11Texture2D* Depth()const noexcept{return depth_.Get();}
    ID3D11Texture2D* Motion()const noexcept{return motion_.Get();}
    ID3D12Fence* ProducerFence()const noexcept{return producer_.Get();}
    uint64_t ProducerValue()const noexcept{return producerValue_;}
    uint32_t Width()const noexcept{return width_;}
    uint32_t Height()const noexcept{return height_;}
    Result<void> TrackReader(ID3D12Fence* fence,uint64_t value){
        if(!track_)return std::unexpected(Error{ErrorKind::InvalidInput,0,"NR prepared lease already consumed/empty"});
        auto result=track_(fence,value);if(result){
            track_={};abandon_={};context_.Reset();color_.Reset();depth_.Reset();motion_.Reset();producer_.Reset();
            source_=epoch_=producerValue_=0;width_=height_=0;reset_=false;
        }return result;
    }
private:
    friend class PreparedBeforeUpscale;
    void Swap(PreparedFsrInput& o)noexcept{using std::swap;swap(source_,o.source_);swap(epoch_,o.epoch_);swap(reset_,o.reset_);swap(width_,o.width_);swap(height_,o.height_);swap(context_,o.context_);swap(color_,o.color_);swap(depth_,o.depth_);swap(motion_,o.motion_);swap(producer_,o.producer_);swap(producerValue_,o.producerValue_);swap(track_,o.track_);swap(abandon_,o.abandon_);}
    uint64_t source_{},epoch_{},producerValue_{};uint32_t width_{},height_{};bool reset_{};
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context_;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> color_,depth_,motion_;
    Microsoft::WRL::ComPtr<ID3D12Fence> producer_;
    std::function<Result<void>(ID3D12Fence*,uint64_t)> track_;
    std::function<void()> abandon_;
};
}
