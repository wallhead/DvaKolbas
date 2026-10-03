#pragma once
#include "PreparedBeforeUpscale.h"
#include "StartupSettings.h"
namespace TheosRenderPipeline::NeuralRendering {
// Caller-serialized real source owner. Inspection does not initialize a vendor.
class BeforeHost {
public:
    BeforeHost();~BeforeHost();
    BeforeHost(const BeforeHost&)=delete;BeforeHost& operator=(const BeforeHost&)=delete;
    Result<void> Inspect(ID3D11Device*,const StartupSettings&,const std::filesystem::path& cache);
    Result<BeforeResult> Evaluate(const BeforeInput&,const SettingsSnapshot&);
    Result<void> Retire();
    bool Available()const;bool Terminal()const;bool Active()const;
    std::string_view ProfileId()const;const std::string& Status()const;
    uint64_t Recorded()const;uint64_t Resets()const;
private:struct State;std::unique_ptr<State> state_;
};
}
