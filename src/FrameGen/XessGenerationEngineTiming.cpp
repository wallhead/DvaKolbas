#include "XessGenerationEngineTiming.h"
#include <limits>
namespace TheosRenderPipeline
{
    using namespace Upscaling;
    namespace
    {
        Result<void> Invalid(const char* reason)
        { return std::unexpected(RuntimeError{ErrorKind::InvalidInput,E_INVALIDARG,reason}); }
    }
    Result<void> XessGenerationEngineTiming::Bind(XellSession* session,bool verified,Mode mode)
    {
        if (latency_) return Invalid("Intel engine timing is already bound");
        if (!session || !session->Context() || !verified)
            return std::unexpected(RuntimeError{ErrorKind::ContextFailure,E_UNEXPECTED,"Intel FG inactive: engine pre-input/simulation/render boundaries are unqualified"});
        latency_=session;verified_=true;mode_=mode;thread_=GetCurrentThreadId();return {};
    }
    Result<void> XessGenerationEngineTiming::Check() const
    {
        if (thread_ && thread_!=GetCurrentThreadId()) return Invalid("Intel engine timing thread differs from the verified source-loop thread");
        if (!verified_ || !latency_ || !latency_->Context() || fault_)
            return std::unexpected(RuntimeError{ErrorKind::ContextFailure,E_UNEXPECTED,"Intel FG inactive: unqualified or failed engine timing owner"});
        return {};
    }
    Result<std::uint32_t> XessGenerationEngineTiming::BeginSourceLoop(std::uint64_t source,std::uint64_t epoch,double* sleepMs,std::uint32_t* sleepSdkId)
    {
        if(sleepMs)*sleepMs=-1;
        if(sleepSdkId)*sleepSdkId=0;
        if (auto ready=Check(); !ready) return std::unexpected(ready.error());
        if (!source || !epoch || phase_ || (source_ && (epoch!=epoch_ || source<=source_)) || next_>std::numeric_limits<uint32_t>::max())
            return std::unexpected(Invalid("Intel timing needs one genuine fresh source; source/SDK epoch changes require a drained reset").error());
        const auto sdkId=static_cast<uint32_t>(next_);
        if (auto result=latency_->BeginFrame(sdkId,sleepMs,sleepSdkId); !result) { fault_=true;return std::unexpected(result.error()); }
        source_=source;epoch_=epoch;id_=sdkId;++next_;phase_=1;sampled_=false;return sdkId;
    }
    Result<void> XessGenerationEngineTiming::InputSampled(std::uint64_t source)
    {
        if (auto ready=Check(); !ready) return ready;
        if (mode_!=Mode::VerifiedInput || !phase_ || phase_!=1 || source!=source_ || sampled_) return Invalid("Intel input proof is only accepted in verified-input mode after this source's sleep");
        sampled_=true;return {};
    }
    Result<void> XessGenerationEngineTiming::SourceMarker(std::uint64_t source,unsigned phase,xell_latency_marker_type_t marker)
    {
        if (auto ready=Check(); !ready) return ready;
        if (source!=source_ || phase_!=phase || (mode_==Mode::VerifiedInput && !sampled_)) return Invalid("Intel source/simulation/render phase does not match the selected timing contract");
        if (auto result=latency_->Marker(id_,marker); !result) { fault_=true;return result; }
        ++phase_;return {};
    }
    Result<void> XessGenerationEngineTiming::EndSimulation(std::uint64_t source)
    { return SourceMarker(source,1,XELL_SIMULATION_END); }
    Result<void> XessGenerationEngineTiming::BeginRender(std::uint64_t source)
    { return SourceMarker(source,2,XELL_RENDERSUBMIT_START); }
    Result<void> XessGenerationEngineTiming::EndRender(std::uint64_t source)
    { return SourceMarker(source,3,XELL_RENDERSUBMIT_END); }
    Result<void> XessGenerationEngineTiming::BeforePresent(std::uint32_t sdkId)
    {
        if (auto ready=Check(); !ready) return ready;
        if (sdkId!=id_) return Invalid("Intel Present ID differs from the engine source ID mapping");
        return SourceMarker(source_,4,XELL_PRESENT_START);
    }
    Result<void> XessGenerationEngineTiming::AfterPresent(std::uint32_t sdkId)
    {
        if (auto ready=Check(); !ready) return ready;
        if (sdkId!=id_) return Invalid("Intel post-Present ID differs from the accepted source");
        if (auto result=SourceMarker(source_,5,XELL_PRESENT_END); !result) return result;
        // Finish this source. Extra/loading Presents cannot manufacture a
        // render boundary or a completed source in either timing mode.
        phase_=0;return {};
    }
    Result<void> XessGenerationEngineTiming::ResetAfterDrain(bool quiescent)
    {
        if (auto ready=Check(); !ready) return ready;
        if (phase_ || !quiescent) return Invalid("Intel timing reset requires completed markers and proven GPU quiescence");
        if (auto result=latency_->ResetAfterDrain(true); !result) { fault_=true;return result; }
        source_=epoch_=id_=phase_=0;next_=1;sampled_=false;return {};
    }
    Result<std::uint32_t> XessGenerationEngineTiming::CurrentId(std::uint64_t source,std::uint64_t epoch) const
    {
        if (auto ready=Check(); !ready) return std::unexpected(ready.error());
        if (!phase_ || source!=source_ || epoch!=epoch_) return std::unexpected(Invalid("Intel host source has no matching active engine timing ID").error());
        return id_;
    }
    Result<void> XessGenerationEngineTiming::AbandonAfterDrain(bool quiescent)
    {
        if (auto ready=Check(); !ready) return ready;
        if (!quiescent) return Invalid("Intel interrupted timing cycle requires a proven GPU drain");
        if (auto result=latency_->ResetAfterDrain(true); !result) { fault_=true;return result; }
        source_=epoch_=id_=phase_=0;sampled_=false;return {};
    }
    Result<void> XessGenerationEngineTiming::AbandonUnsubmitted(bool noTaggedWork)
    {
        if (auto ready=Check(); !ready) return ready;
        if (!noTaggedWork) return Invalid("Intel submitted cycle requires drained retirement");
        if (auto result=latency_->AbandonUnsubmittedFrame(); !result) { fault_=true;return result; }
        // Keep source, epoch and next SDK ID. The skipped cycle cannot be reused.
        id_=phase_=0;sampled_=false;return {};
    }
    Result<std::uint32_t> XessGenerationEngineTiming::CurrentRenderId(std::uint64_t source,std::uint64_t epoch) const
    {
        auto id=CurrentId(source,epoch);if(!id)return id;
        if(phase_!=3 || (mode_==Mode::VerifiedInput && !sampled_))return std::unexpected(Invalid("Intel tagging requires the selected timing contract and a genuine render-start boundary").error());
        return id;
    }
}
