#pragma once
#include "Stage.h"
#include <d3d11_4.h>
namespace TheosRenderPipeline::NeuralRendering {
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
struct BeforeResult {bool evaluated{},effectiveReset{};};
class BeforeUpscale {
public:
    BeforeUpscale();~BeforeUpscale();
    BeforeUpscale(const BeforeUpscale&)=delete;
    BeforeUpscale& operator=(const BeforeUpscale&)=delete;
    // Research baseline: native FP16 linear world, R32 depth, RG16 motion,
    // same immediate D3D11 context. Caller serializes all calls and unbinds
    // writable views before Evaluate. No UI/After or game color preparation.
    Result<void> Initialize(std::shared_ptr<RuntimeOwner>,ID3D11Device*,const StageContract&,unsigned preset=0);
    // Return success only after the copy back and its actual D3D11 reader
    // have completed. This first bridge prioritizes proof over pipelining.
    Result<BeforeResult> Evaluate(const BeforeInput&,const SettingsSnapshot&);
    Result<void> Retire();
    StageDiagnostics Diagnostics()const;
private:
    struct State;std::unique_ptr<State> state_;
};
}
