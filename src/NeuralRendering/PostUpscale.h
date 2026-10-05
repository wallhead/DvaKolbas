#pragma once
#include "PreparedBeforeUpscale.h"
#include "PostSrContract.h"
namespace TheosRenderPipeline::NeuralRendering {
// Source-thread adapter only. It never processes a generated presentation.
struct PostSrInput { BeforeInput resources; PostSrSourceContract source; };
class PostUpscale {
public:
    Result<void> Initialize(std::shared_ptr<RuntimeOwner>,ID3D11Device*,const StageContract&,
        unsigned preset=0,PerformanceMetrics* metrics=nullptr,ColorDomain domain=ColorDomain::SdrBytes,unsigned passes=1);
    Result<BeforeResult> Evaluate(const PostSrInput&,const SettingsSnapshot&);
    Result<void> WaitDelivery(const BeforeResult& result){return bridge_.WaitDelivery(result);}
    Result<void> TrackReader(const DeliveryTicket& ticket,ID3D12Fence* fence,uint64_t value){return bridge_.TrackReader(ticket,fence,value);}
    Result<uint32_t> CollectCompleted(){return bridge_.CollectCompleted();}
    Result<void> Retire(){return bridge_.Retire();}
    StageDiagnostics Diagnostics()const{return bridge_.Diagnostics();}
private:
    PreparedBeforeUpscale bridge_;
    ColorDomain domain_{ColorDomain::Unknown};
};
}
