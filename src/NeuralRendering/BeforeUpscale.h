#pragma once
#include "Stage.h"
#include <d3d11_4.h>
namespace TheosRenderPipeline::NeuralRendering {
class PerformanceQueries;
struct BeforeInput {
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> color,depth,motion;
    uint64_t epoch{},sourceId{},previousSourceId{},guideEpoch{},guideSourceId{};
    double presentationTime{};
    ImageExtent colorExtent,guideExtent;
    ColorDomain colorDomain{ColorDomain::Unknown};
    float motionScaleX{},motionScaleY{};
    bool reset{},depthInverted{};
};
class DeliveryTicket {
public:
    uint64_t SourceId()const noexcept{return source_;}
    uint64_t Epoch()const noexcept{return epoch_;}
private:
    friend class BeforeUpscale;friend class PreparedBeforeUpscale;
    Detail::TicketOwnership::Token owner_;
    uint64_t id_{},source_{},epoch_{};size_t slot_{};
};
struct BeforeResult {bool evaluated{},effectiveReset{};DeliveryTicket delivery;};
class BeforeUpscale {
public:
    BeforeUpscale();~BeforeUpscale();
    BeforeUpscale(const BeforeUpscale&)=delete;
    BeforeUpscale& operator=(const BeforeUpscale&)=delete;
    // Native FP16 linear or explicit SDR RGBA8 world, R32 depth, RG16 motion,
    // same immediate D3D11 context. Caller serializes all calls and unbinds
    // writable views before Evaluate. Placement is latched at initialization;
    // default Before keeps existing callers strict. No UI or game preparation.
    Result<void> Initialize(std::shared_ptr<RuntimeOwner>,ID3D11Device*,const StageContract&,unsigned preset=0,PerformanceMetrics* metrics=nullptr,PerformanceQueries* queries11=nullptr,ColorDomain domain=ColorDomain::Linear,Placement placement=Placement::Before,unsigned passes=1);
    // Success means delivery was queued on the authoritative immediate context.
    // Readback callers explicitly WaitDelivery; later work on that context is ordered.
    Result<BeforeResult> Evaluate(const BeforeInput&,const SettingsSnapshot&);
    Result<uint32_t> CollectCompleted();
    Result<void> WaitDelivery(const BeforeResult&);
    Result<void> TrackReader(const DeliveryTicket&,ID3D12Fence*,uint64_t value);
    Result<void> Retire();
    bool InitializationRolledBackBeforeCreate()const noexcept;
    StageDiagnostics Diagnostics()const;
private:
    struct State;std::unique_ptr<State> state_;
};
}
