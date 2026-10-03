#pragma once
#include "BeforeUpscale.h"
#include "Upscaling/FSRColorContract.h"
namespace TheosRenderPipeline::NeuralRendering {
// Real source preparation, independent of DLSS-G and the selected SR backend.
// SDR transfer functions reuse the accepted FSR converter. Extended HDR and
// reduced model reconstruction remain separate, unsupported contracts here.
class PreparedBeforeUpscale {
public:
    PreparedBeforeUpscale();~PreparedBeforeUpscale();
    PreparedBeforeUpscale(const PreparedBeforeUpscale&)=delete;
    PreparedBeforeUpscale& operator=(const PreparedBeforeUpscale&)=delete;
    Result<void> Initialize(std::shared_ptr<RuntimeOwner>,ID3D11Device*,const StageContract&,unsigned preset=0);
    Result<BeforeResult> Evaluate(const BeforeInput&,Upscaling::ColorEncoding,const SettingsSnapshot&);
    Result<void> Retire();
    StageDiagnostics Diagnostics()const;
private:
    struct State;std::unique_ptr<State> state_;
};
}
