#pragma once
#include "PostUpscale.h"
#include "StartupSettings.h"
namespace TheosRenderPipeline::NeuralRendering {
// Caller-serialized real source owner. Inspection does not initialize a vendor.
class BeforeHost {
public:
    BeforeHost();~BeforeHost();
    BeforeHost(const BeforeHost&)=delete;BeforeHost& operator=(const BeforeHost&)=delete;
    // A presenter supplies its retained D3D12 device to avoid creating a second
    // injector proxy around the adapter's existing native device.
    Result<void> Inspect(ID3D11Device*,const StartupSettings&,const std::filesystem::path& cache,ID3D12Device* presenter=nullptr);
    Result<BeforeResult> Evaluate(const BeforeInput&,const SettingsSnapshot&,PreparedFsrInput* linearOutput=nullptr);
    Result<BeforeResult> EvaluatePost(const PostSrInput&,const SettingsSnapshot&);
    Result<void> Retire();
    const AdapterIdentity& RenderAdapter()const;
    bool Available()const;bool Terminal()const;bool Active()const;
    std::string_view ProfileId()const;const std::string& Status()const;
    const std::filesystem::path& DriverCorePath()const;
    uint64_t Recorded()const;uint64_t Resets()const;
private:
    Result<BeforeResult> EvaluateSource(const BeforeInput&,const SettingsSnapshot&,PreparedFsrInput*,const PostSrSourceContract*);
    struct State;std::unique_ptr<State> state_;
};
}
