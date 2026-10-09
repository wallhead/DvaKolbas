#pragma once
#include "RendererSettings.h"
#include <optional>

namespace TheosRenderPipeline {
class RendererSettingsEditTransaction {
    std::optional<RendererSettingsDraft> before_;
    std::uint32_t active_{};

  public:
    template <class Apply>
    bool Observe(const RendererSettingsDraft &before, const RendererSettingsDraft &after,
                 std::uint32_t active, Apply &&apply) {
        const bool edited =
            before.valid && after.valid &&
            (CountRendererSettingsChanges(after, before) != 0 || after.sharpness != before.sharpness);
        bool committed = false;
        // A direct handoff to another editor must finish the previous interaction
        // using its value before this frame's new widget changes.
        if (before_ && active && active_ && active != active_)
            committed = Commit(before, apply);
        if (!before_ && edited) {
            before_ = before;
            active_ = active;
        }
        return (!active && Commit(after, std::forward<Apply>(apply))) || committed;
    }
    template <class Apply> bool Commit(const RendererSettingsDraft &after, Apply &&apply) {
        if (!before_)
            return false;
        auto before = std::move(*before_);
        before_.reset();
        active_ = 0;
        return ApplyRendererSettingsEdits(before, after, [&] { apply(before, after); });
    }
};

// Copy this interaction's live edits onto the actual owner's settings. Pending
// allocation choices and unchanged rejected inputs never enter this transaction.
inline RendererSettingsDraft ProjectRendererLiveEdits(const RendererSettingsDraft &before,
                                                      const RendererSettingsDraft &after,
                                                      RendererSettingsDraft current,
                                                      const RendererSettingsDraft *accepted = nullptr) {
    const auto &values = accepted ? *accepted : after;
    const auto copy = [accepted](const auto &a, const auto &b, auto &out, const auto &source,
                                 auto... members) {
        const auto field = [&](auto member) {
            if (b.*member != a.*member && (!accepted || out.*member == b.*member))
                out.*member = source.*member;
        };
        (field(members), ...);
    };
    copy(before, after, current, values, &RendererSettingsDraft::dlssPreset,
         &RendererSettingsDraft::autoExposure, &RendererSettingsDraft::sharpening,
         &RendererSettingsDraft::sharpness, &RendererSettingsDraft::enableJitter,
         &RendererSettingsDraft::nativeUI, &RendererSettingsDraft::requestLoadingArtwork,
         &RendererSettingsDraft::reShadeBeforeUpscaling, &RendererSettingsDraft::lateOverlayBridge,
         &RendererSettingsDraft::enableGPUTimings, &RendererSettingsDraft::enableFrameTrace,
         &RendererSettingsDraft::directRCASOutput, &RendererSettingsDraft::directDLSSOutput,
         &RendererSettingsDraft::appearance);
    if (before.textureProviderConnected && after.textureProviderConnected && current.textureProviderConnected)
        copy(before, after, current, values, &RendererSettingsDraft::textureProviderSettings);
    copy(before.fsr, after.fsr, current.fsr, values.fsr, &Upscaling::FsrSettings::sharpness);
    using P = SourceDLSSG::Preferences;
    copy(before.sourceDLSSG, after.sourceDLSSG, current.sourceDLSSG, values.sourceDLSSG, &P::reflexMode,
         &P::outputFPSLimit, &P::uiRecomposition, &P::neuralBeforeUpscaling, &P::neuralPasses,
         &P::neuralCombat);
    const auto tuning = [&](const auto &a, const auto &b, auto &out, const auto &source) {
        using T = NeuralRendering::Tuning;
        copy(a, b, out, source, &T::style, &T::intensity, &T::localToneStrength, &T::localStructureStrength,
             &T::skinStructureStrength, &T::useAutoSkinMask, &T::uiCorrection);
    };
    tuning(before.sourceDLSSG.neuralTuning, after.sourceDLSSG.neuralTuning, current.sourceDLSSG.neuralTuning,
           values.sourceDLSSG.neuralTuning);
    using R = NeuralRendering::Reconstruction;
    copy(before.sourceDLSSG.neuralReconstruction, after.sourceDLSSG.neuralReconstruction,
         current.sourceDLSSG.neuralReconstruction, values.sourceDLSSG.neuralReconstruction, &R::preset,
         &R::method, &R::inputScale, &R::transferStrength, &R::colourStrength, &R::maxRatio, &R::whitePoint,
         &R::colorIsHDR, &R::peripheralCompression, &R::fusedPreparation, &R::producerColor);
    const auto pass = [&](const auto &a, const auto &b, auto &out, const auto &source) {
        using S = NeuralRendering::SecondPassSettings;
        copy(a, b, out, source, &S::linked, &S::preset, &S::inputScale);
        tuning(a.tuning, b.tuning, out.tuning, source.tuning);
    };
    pass(before.sourceDLSSG.neuralSecondPass, after.sourceDLSSG.neuralSecondPass,
         current.sourceDLSSG.neuralSecondPass, values.sourceDLSSG.neuralSecondPass);
    pass(before.sourceDLSSG.neuralThirdPass, after.sourceDLSSG.neuralThirdPass,
         current.sourceDLSSG.neuralThirdPass, values.sourceDLSSG.neuralThirdPass);
    using G = SourceDLSSG::GenerationRequest;
    copy(before.sourceDLSSG.generation, after.sourceDLSSG.generation, current.sourceDLSSG.generation,
         values.sourceDLSSG.generation, &G::generatedFrames, &G::dynamicTargetFPS, &G::dynamic);
    const bool ownerChoice =
        before.upscaleType != after.upscaleType || before.generationBackend != after.generationBackend;
    if (!ownerChoice) {
        copy(before, after, current, values, &RendererSettingsDraft::generationEnabled);
        copy(before.sourceDLSSG, after.sourceDLSSG, current.sourceDLSSG, values.sourceDLSSG,
             &P::neuralEnabled);
    }
    using H = HDROutput::Settings;
    copy(before.sourceDLSSG.hdrOutput, after.sourceDLSSG.hdrOutput, current.sourceDLSSG.hdrOutput,
         values.sourceDLSSG.hdrOutput, &H::matchWindowsSDR, &H::paperWhiteNits, &H::peakNits, &H::uiNits,
         &H::highlightStrength, &H::expansionStart, &H::transfer);
    return current;
}

inline void StageRendererUpscaleProvider(RendererSettingsDraft &draft, bool fsr) {
    const bool neural = draft.sourceDLSSG.neuralEnabled;
    SetRendererUpscaleProvider(draft, fsr);
    // The NR checkbox addresses this session even while another provider is staged.
    draft.sourceDLSSG.neuralEnabled = neural;
}
inline RendererSettingsDraft PrepareRendererStartupDraft(RendererSettingsDraft draft, bool community) {
    if(draft.upscaleType==Xess){draft.generationBackend=0;draft.generationEnabled=false;draft.sourceDLSSG.neuralEnabled=false;draft.sourceDLSSG.hdrOutput.enabled=false;draft.dynamicResolution=false;draft.enableJitter=true;}
    if (draft.upscaleType == FSR) {
        if (draft.generationBackend == 0)
            draft.generationEnabled = false;
        draft.sourceDLSSG.hdrOutput.enabled = false;
        draft.dynamicResolution = false;
        if (!community)
            draft.sourceDLSSG.neuralEnabled = false;
    }
    return draft;
}
} // namespace TheosRenderPipeline
