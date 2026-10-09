#pragma once

#include "DLSSPreset.h"
#include "Upscaling/FSRSettings.h"
#include "Upscaling/XessSettings.h"
#include <algorithm>

namespace TheosRenderPipeline::Upscaler
{
    // Preserve existing RaZkolbaS INI mode values: 0 = DLSS, 3 = DLAA.
    struct Creation
    {
        int mode{0}, quality{2}, preset{11};
        bool sharpening{true}, autoExposure{true};
        Upscaling::FsrSettings fsr;
        Upscaling::XessSettings xess;
        bool operator==(const Creation&) const = default;
        int AllocationQuality() const { return mode == 3 ? 5 : quality; }
    };

    inline Creation Sanitize(Creation value)
    {
        value.mode = value.mode == 3 ? 3 : value.mode == 4 ? 4 : value.mode == 5 ? 5 : 0;
        value.quality = std::clamp(value.quality, 0, 4);
        value.preset = TheosRenderPipeline::DLSSPreset::Sanitize(value.preset);
        return value;
    }

    class Configuration
    {
    public:
        void Initialize(Creation value)
        {
            requested_ = submitted_ = effective_ = persisted_ = startup_ = Sanitize(value);
            startupRequest_=startup_;fallback_=false;
            initialized_ = true;
            ready_ = failed_ = false;
        }
        void Request(Creation value) { requested_ = Sanitize(value); }
        bool UseStartupFallback(Creation value)
        {
            if(!initialized_ || ready_ || failed_)return false;
            startup_=submitted_=effective_=Sanitize(value);fallback_=true;return true;
        }
        void Saved() { persisted_ = requested_; }
        const Creation& Requested() const { return requested_; }
        const Creation& Submitted() const { return submitted_; }
        const Creation& Effective() const { return effective_; }
        const Creation& Persisted() const { return persisted_; }
        const Creation& Startup() const { return startup_; }
        bool Initialized() const { return initialized_; }
        bool Ready() const { return ready_ && !failed_; }
        bool Failed() const { return failed_; }
        bool Unsaved() const { return requested_ != persisted_; }
        bool NeedsRestart() const
        {
            return requested_.mode != startupRequest_.mode ||
                requested_.fsr.generationProviderPolicy != startupRequest_.fsr.generationProviderPolicy ||
                requested_.fsr.sourceColorEncoding != startupRequest_.fsr.sourceColorEncoding ||
                (requested_.mode == 5 ? requested_.xess.quality != startupRequest_.xess.quality || requested_.xess.sourceEncoding != startupRequest_.xess.sourceEncoding : requested_.mode == 4 ? requested_.fsr.quality != startupRequest_.fsr.quality || requested_.fsr.providerPolicy != startupRequest_.fsr.providerPolicy ||
                requested_.fsr.sourceColorEncoding != startupRequest_.fsr.sourceColorEncoding ||
                requested_.fsr.generationProviderPolicy != startupRequest_.fsr.generationProviderPolicy :
                requested_.mode != 3 && requested_.quality != startupRequest_.quality);
        }
        Creation LiveCandidate() const
        {
            if(fallback_ && requested_.mode!=startup_.mode)return effective_;
            auto value = requested_;
            value.mode = startup_.mode;
            value.quality = startup_.quality;
            value.fsr.quality = startup_.fsr.quality;
            value.fsr.providerPolicy = startup_.fsr.providerPolicy;
            value.fsr.generationProviderPolicy = startup_.fsr.generationProviderPolicy;
            value.fsr.sourceColorEncoding = startup_.fsr.sourceColorEncoding;
            value.xess = startup_.xess;
            if(startup_.mode==5)value.xess.sharpness=requested_.xess.sharpness;
            if (startup_.mode == 4 || startup_.mode == 5) {
                value.preset = startup_.preset; value.autoExposure = startup_.autoExposure; value.sharpening = startup_.sharpening;
            } else { value.fsr = startup_.fsr; }
            return value;
        }
        bool NeedsLiveChange() const { return Ready() && LiveCandidate() != effective_; }
        Creation BeginSubmission() { return submitted_ = LiveCandidate(); }
        void Completed(bool success)
        {
            ready_ = success;
            failed_ = !success;
            if (success) { effective_ = submitted_; }
        }
    private:
        Creation requested_, submitted_, effective_, persisted_, startup_, startupRequest_;
        bool initialized_{}, ready_{}, failed_{}, fallback_{};
    };

    // One production/test boundary: a failed retirement must never reach feature
    // creation, and failed creation/resume must never claim effective settings.
    template<class Retire, class Create, class Resume>
    bool ApplyLive(Configuration& configuration, Retire retire, Create create, Resume resume)
    {
        if (!configuration.NeedsLiveChange()) { return !configuration.Failed(); }
        if (!retire()) { configuration.Completed(false); return false; }
        const auto request = configuration.BeginSubmission();
        if (!create(request) || !resume()) { configuration.Completed(false); return false; }
        configuration.Completed(true);
        return true;
    }
}
