#include "RendererSettings.h"
#include "RendererSettingsAction.h"
#include "FrameGen/SourceFrameGeneration.h"
#include <SimpleIni.h>
#include <iostream>
#include <stdexcept>
#include <vector>

using namespace TheosRenderPipeline;
namespace {
void Require(bool value, const char* message) { if (!value) { throw std::runtime_error(message); } }
void Feedback()
{
    for (const bool save : {false, true}) {
        std::vector<std::string> log;
        const auto result = RejectSettingsAction(save, "Dynamic target must exceed 60 FPS.",
            [&](const std::string& message) { log.push_back(message); });
        Require(result.error && !result.applied, "rejected action keeps the draft unapplied");
        Require(log.size() == 1 && log[0].find(save ? "Save as default rejected" : "Apply rejected") != std::string::npos &&
            log[0].find(result.message) != std::string::npos, "log records action and rejection reason");
        const auto status = SettingsStatus(4, result.message, result.error);
        Require(status.kind == SettingsStatusKind::Error && status.text == result.message,
            "rejection remains visible with pending edits");
    }
    Require(SettingsStatus(2, "Settings saved.", false).kind == SettingsStatusKind::Pending, "new edits supersede old success");
    Require(SettingsStatus(0, "Settings saved.", false).kind == SettingsStatusKind::Success, "successful save remains visible");
    Require(SettingsStatus(0, "", false).kind == SettingsStatusKind::Neutral, "idle footer");
}
void ImmediateMenuEdits()
{
    RendererSettingsDraft active; active.valid = true;
    auto draft = active;
    RendererSettingsCapabilities caps{true,true,true,false};
    int applications = 0;
    const auto apply = [&] {
        ++applications;
        if (!ValidateRendererSettings(draft, caps, &active)) {
            active = draft;
            RefreshAppliedRendererSettingsDraft(draft, active);
        }
    };
    auto before = draft;
    Require(!ApplyRendererSettingsEdits(before, draft, apply) && applications == 0,
        "opening or redrawing the menu cannot apply settings");
    draft.sourceDLSSG.neuralEnabled = true;
    Require(ApplyRendererSettingsEdits(before, draft, apply) && active.sourceDLSSG.neuralEnabled && applications == 1,
        "NR checkbox applies without an Apply button");
    before = draft;
    draft.sourceDLSSG.generation.dynamic = true;
    draft.sourceDLSSG.generation.dynamicTargetFPS = 12;
    Require(ApplyRendererSettingsEdits(before, draft, apply) && applications == 2 &&
        !active.sourceDLSSG.generation.dynamic, "invalid edits preserve the active configuration");
    before = draft;
    for (int frame = 0; frame != 120; ++frame)
        Require(!ApplyRendererSettingsEdits(before, draft, apply), "a rejected edit cannot retry each redraw");
    Require(applications == 2, "passive menu frames cannot repeat rejected applications");
    draft.sourceDLSSG.generation.dynamicTargetFPS = 120;
    Require(ApplyRendererSettingsEdits(before, draft, apply) && applications == 3 &&
        active.sourceDLSSG.generation.dynamicTargetFPS == 120, "corrected edits apply on the next interaction");
    draft.sourceDLSSG.hdrOutput.enabled = true;
    draft.fsr.sourceColorEncoding = Upscaling::ColorEncoding::Gamma22;
    before = draft;
    SetRendererUpscaleMode(draft, FSR);
    caps.fsrBuilt = true;
    Require(ApplyRendererSettingsEdits(before, draft, apply) && active.upscaleType == FSR,
        "provider changes automatically stage valid startup settings");
    before = draft;
    SetRendererUpscaleMode(draft, DLSS);
    Require(draft.sourceDLSSG.neuralEnabled && draft.sourceDLSSG.hdrOutput.enabled,
        "automatic recapture preserves the NVIDIA choices across provider changes");
    Require(ApplyRendererSettingsEdits(before, draft, apply), "returning to DLSS applies restored choices");
}
void NativeRenderScale()
{
    RendererSettingsDraft before; before.valid = true;
    auto preciseEdit = before;
    preciseEdit.sharpness = std::nextafter(before.sharpness, 1.0f);
    bool applied = false;
    Require(ApplyRendererSettingsEdits(before, preciseEdit, [&] { applied = true; }) && applied,
        "fine sharpness edits cannot disappear between UI frames");
    RendererSettingsDraft draft; draft.valid = true; draft.qualityLevel = 4;
    draft.sourceDLSSG.neuralEnabled = true;
    SetNvidiaRenderScale(draft, -1);
    Require(draft.upscaleType == DLAA && draft.qualityLevel == 4 && draft.sourceDLSSG.neuralEnabled,
        "Native scale selects the compatible DLAA INI mode and preserves other settings");
    for (int quality : {3,0,1,2,4}) {
        SetNvidiaRenderScale(draft, quality);
        Require(draft.upscaleType == DLSS && draft.qualityLevel == quality && draft.sourceDLSSG.neuralEnabled,
            "every scaled option leaves Native and selects the existing DLSS quality ID");
        SetNvidiaRenderScale(draft, -1);
        Require(draft.upscaleType == DLAA && draft.qualityLevel == quality,
            "returning to Native preserves the last scaled quality");
    }
    SetRendererUpscaleProvider(draft, true);
    Require(draft.upscaleType == FSR, "FSR provider can replace Native DLSS");
    auto captured = draft; captured.nvidiaMode = {}; captured.fsrMode = {};
    RefreshAppliedRendererSettingsDraft(draft, captured);
    SetRendererUpscaleProvider(draft, false);
    Require(draft.upscaleType == DLAA && draft.sourceDLSSG.neuralEnabled,
        "returning to DLSS restores Native with the NVIDIA NR preference");
    SetNvidiaRenderScale(draft, 2);
    SetRendererUpscaleProvider(draft, true);
    SetRendererUpscaleProvider(draft, false);
    Require(draft.upscaleType == DLSS && draft.qualityLevel == 2,
        "provider round trip also preserves a scaled DLSS choice");
}
void Generation()
{
    using namespace SourceDLSSG;
    for (const unsigned count : {1u, 3u, 5u}) for (const bool dynamic : {false, true}) {
        for (const unsigned target : {0u, 1u, 60u, 61u, 165u, 1000u, 1001u}) {
            GenerationRequest request{count, target, dynamic};
            Require(ValidGenerationRequest(request) == (!dynamic || target == 0 || (target >= 61 && target <= 1000)),
                "only active dynamic targets block validation");
            Preferences preferences; preferences.generation = request;
            CSimpleIniA ini; StorePreferences(ini, preferences); IniLayout::StoreCanonical(ini);
            ini.SetLongValue("FrameGeneration", "DynamicTargetFPS", target); // Also cover manually edited invalid INIs.
            const auto loaded = LoadPreferences(ini).generation;
            Require(loaded.generatedFrames == count && loaded.dynamic == dynamic, "invalid target cannot discard multiplier or dynamic selection");
            Require(loaded.dynamicTargetFPS == (ValidDynamicTarget(target) ? target : 0), "invalid target recovers to display refresh");
            Require(SelectGeneration(loaded, 5, true).effective.dynamicTargetFPS == (dynamic ? loaded.dynamicTargetFPS : 0),
                "fixed mode never submits a hidden target to runtime");
        }
    }
    Require(!ValidGenerationRequest({0,0,false}) && !ValidGenerationRequest({6,0,false}), "bad multipliers remain invalid");
    {
        CSimpleIniA ini;
        Require(LoadPreferences(ini).uiRecomposition, "existing INIs without the key get UI recomposition on");
        Preferences preferences; preferences.uiRecomposition = false;
        StorePreferences(ini, preferences); IniLayout::StoreCanonical(ini);
        Require(!LoadPreferences(ini).uiRecomposition, "a saved opt-out round trips");
        RendererSettingsDraft current; current.valid = true;
        auto draft = current; draft.sourceDLSSG.uiRecomposition = false;
        Require(CountRendererSettingsChanges(draft, current) == 1, "UI recomposition participates in Apply/Discard");
    }
}
void Neural()
{
    RendererSettingsDraft legacy; legacy.valid = true; legacy.sourceDLSSG.neuralEnabled = true;
    RendererSettingsCapabilities legacyCaps{true,true,true,false};
    legacy.sourceDLSSG.neuralPasses = 2;
    Require(!ValidateRendererSettings(legacy, legacyCaps), "legacy renderer accepts two passes");
    legacy.sourceDLSSG.neuralPasses = 3;
    Require(!ValidateRendererSettings(legacy, legacyCaps), "legacy Apply preserves a saved third pass while execution caps at two");
    auto thirdDraft = legacy; thirdDraft.sourceDLSSG.neuralThirdPass.tuning.intensity = .5f;
    Require(!SameNeuralPreferences(legacy.sourceDLSSG, thirdDraft.sourceDLSSG) &&
        CountRendererSettingsChanges(thirdDraft, legacy) == 1, "third-pass edits participate in Apply and capability validation");
    RendererSettingsDraft current; current.valid = true; current.sourceDLSSG.neuralEnabled = true;
    auto draft = current; draft.upscaleType = DLAA; draft.qualityLevel = 4;
    RendererSettingsCapabilities lostUI{true,true,false,false};
    Require(ValidateRendererSettings(draft, lostUI) != nullptr, "new unavailable NR requests rejected");
    Require(ValidateRendererSettings(draft, lostUI, &current) == nullptr, "unchanged NR cannot block unrelated DLAA save");
    auto thirdOnly = current; thirdOnly.sourceDLSSG.neuralThirdPass.tuning.intensity = .5f;
    Require(ValidateRendererSettings(thirdOnly, lostUI, &current), "third-pass edits cannot bypass lost NR capability validation");
    Require(NeuralSettingsUnavailable(draft.upscaleType, lostUI) != nullptr, "preserved request cannot enable execution without composition");
    draft.sourceDLSSG.neuralPasses = 2;
    Require(ValidateRendererSettings(draft, lostUI, &current) != nullptr, "cannot reconfigure enabled NR after capability loss");
    auto combatDraft = current;
    combatDraft.sourceDLSSG.neuralCombat.inCombat = true;
    Require(CountRendererSettingsChanges(combatDraft, current) == 1, "combat edit participates in Apply/Discard pending changes");
    Require(!SameNeuralPreferences(combatDraft.sourceDLSSG, current.sourceDLSSG) &&
        ValidateRendererSettings(combatDraft, lostUI, &current), "combat edits obey NR availability validation");
    draft.sourceDLSSG.neuralEnabled = false;
    Require(ValidateRendererSettings(draft, lostUI, &current) == nullptr, "turn NR off despite unavailable composition");
    for (const bool enabled : {false,true}) for (const bool available : {false,true}) {
        Require(CanEditNeuralEnabled(enabled, available) == (enabled || available), "off action available, unavailable activation disabled");
    }
    RendererSettingsCapabilities failed{true,true,true,false,false};
    draft = current;
    Require(!ValidateRendererSettings(draft, failed, &current) && NeuralSettingsUnavailable(draft.upscaleType, failed),
        "unchanged NR request can persist but cannot resume after runtime failure");
    auto off = current; off.sourceDLSSG.neuralEnabled = false;
    Require(ValidateRendererSettings(draft, failed, &off), "off-to-on request after failure still rejected");
    failed.sourceHost = false;
    Require(ValidateRendererSettings(draft, failed, &current), "unavailable source host still rejected");
}
void CommunityNeural()
{
    RendererSettingsCapabilities caps{true,true,true,false};
    caps.fsrBuilt=true; caps.fsrFgBuilt=true; caps.communityNeural=true;
    RendererSettingsDraft draft; draft.valid=true; draft.upscaleType=FSR;
    draft.generationBackend=2; draft.fsr.sourceColorEncoding=Upscaling::ColorEncoding::Gamma22;
    draft.sourceDLSSG.neuralEnabled=true; draft.sourceDLSSG.neuralBeforeUpscaling=true;
    draft.sourceDLSSG.neuralPasses=1; draft.sourceDLSSG.neuralReconstruction={};
    draft.sourceDLSSG.neuralReconstruction.inputScale=1;
    Require(!ValidateRendererSettings(draft,caps),"community NR Before works with FSR FG");
    auto after=draft; after.sourceDLSSG.neuralBeforeUpscaling=false;
    Require(!ValidateRendererSettings(after,caps),"reduced-resolution FSR After uses qualified render guides");
    for(auto quality : {Upscaling::Quality::Quality,Upscaling::Quality::Balanced,Upscaling::Quality::Performance,Upscaling::Quality::NativeAA}) {
        after.fsr.quality=quality;
        Require(!ValidateRendererSettings(after,caps),"fixed FSR quality permits After NR");
    }
    after.fsr.quality=Upscaling::Quality::NativeAA;
    Require(!ValidateRendererSettings(after,caps),"Native AA permits source NR after FSR before FG");
    after.upscaleType=DLAA;after.generationBackend=1;
    Require(!ValidateRendererSettings(after,caps),"DLAA permits source NR after reconstruction");
    after.upscaleType=DLSS;
    Require(!ValidateRendererSettings(after,caps),"scaled DLSS After uses qualified render guides");
    after.dynamicResolution=true;
    Require(ValidateRendererSettings(after,caps),"dynamic guide sizes remain unavailable for After NR");
    auto twice=draft; twice.sourceDLSSG.neuralPasses=2;
    Require(!ValidateRendererSettings(twice,caps),"community renderer accepts two native passes");
    auto thrice=draft; thrice.sourceDLSSG.neuralPasses=3;
    Require(!ValidateRendererSettings(thrice,caps),"community renderer accepts three native passes");
    thrice.sourceDLSSG.neuralThirdPass.linked=false;
    thrice.sourceDLSSG.neuralThirdPass.inputScale=.5f;
    Require(ValidateRendererSettings(thrice,caps),"community renderer rejects reduced pass-three model input");
    auto reduced=draft; reduced.sourceDLSSG.neuralReconstruction.inputScale=0.5f;
    Require(ValidateRendererSettings(reduced,caps),"reduced model cannot silently run native");
    caps.neuralRuntime=false; draft.sourceDLSSG.neuralEnabled=false;
    Require(!ValidateRendererSettings(draft,caps),"NR off is available after runtime loss");
    caps.communityNeural=false; caps.neuralRuntime=true; draft.sourceDLSSG.neuralEnabled=true;
    Require(ValidateRendererSettings(draft,caps),"legacy FSR NR remains rejected");
}
void LiveGenerationActions()
{
    auto& generation=*SourceFrameGeneration::GetSingleton();
    for (bool fsr : {false,true}) for (bool initial : {false,true}) for (bool save : {false,true}) {
        generation.RequestRuntimeInterpolation(initial);
        RendererSettingsDraft draft; draft.valid=true;
        draft.generationEnabled=generation.RuntimeInterpolationRequested();
        draft.generationBackend=fsr?2:1;draft.upscaleType=fsr?FSR:DLSS;
        draft.fsr.sourceColorEncoding=TheosRenderPipeline::Upscaling::ColorEncoding::Gamma22;
        CSimpleIniA defaults; defaults.SetBoolValue("FrameGeneration","Enabled",initial);
        const bool selected=!initial;
        SetLiveGenerationRequest(draft,generation,selected,fsr?2:1);
        Require(generation.RuntimeInterpolationRequested()==selected,"checkbox takes effect immediately");
        // An unrelated edit followed by Apply/Save must preserve the checkbox.
        draft.autoExposure=false;
        RendererSettingsCapabilities caps{true,false,true,false};caps.fsrBuilt=true;caps.fsrFgBuilt=true;
        Require(!ValidateRendererSettings(draft,caps),"NVIDIA/AMD draft remains valid");
        ApplyRendererGeneration(draft,generation,fsr?2:1);
        if (save) generation.StoreInterpolationPreference(defaults);
        Require(generation.RuntimeInterpolationRequested()==selected,"Apply/Save cannot reverse live FG toggle");
        Require(defaults.GetBoolValue("FrameGeneration","Enabled")== (save?selected:initial),
            "Save persists the visible choice; Apply leaves startup defaults alone");
        if(save) {
            generation.LoadStartupPreferences(defaults);
            Require(generation.RuntimeInterpolationRequested()==selected,"saved toggle survives next startup");
        }
    }
}
void DraftModeRoundTrip()
{
    for(int mode:{DLSS,DLAA}) for(bool requested:{false,true}) {
        RendererSettingsDraft draft;draft.valid=true;draft.upscaleType=mode;
        draft.generationEnabled=requested;draft.sourceDLSSG.neuralEnabled=requested;
        draft.sourceDLSSG.hdrOutput.enabled=requested;
        draft.dynamicResolution=requested;
        SetRendererUpscaleMode(draft,FSR);
        Require(!draft.generationEnabled&&!draft.sourceDLSSG.neuralEnabled&&!draft.sourceDLSSG.hdrOutput.enabled&&!draft.dynamicResolution,
            "FSR mode stages compatible ordinary defaults");
        SetRendererUpscaleMode(draft,mode);
        Require(draft.generationEnabled==requested&&draft.sourceDLSSG.neuralEnabled==requested&&draft.sourceDLSSG.hdrOutput.enabled==requested&&draft.dynamicResolution==requested,
            "returning to NVIDIA draft restores FG NR and HDR choices");
    }
    RendererSettingsDraft fsr;fsr.upscaleType=FSR;fsr.generationBackend=2;fsr.generationEnabled=true;
    fsr.sourceDLSSG.neuralEnabled=true;
    SetRendererUpscaleMode(fsr,DLAA);SetRendererUpscaleMode(fsr,FSR);
    Require(fsr.generationBackend==2&&fsr.generationEnabled&&fsr.sourceDLSSG.neuralEnabled,
        "FSR draft round trip retains explicitly selected FG presenter and community NR");
}
void StagedGenerationDefaults()
{
    auto& generation=*SourceFrameGeneration::GetSingleton();
    generation.RequestRuntimeInterpolation(true);
    generation.settings.enabled=false;
    CSimpleIniA defaults;generation.StoreInterpolationPreference(defaults);
    Require(!defaults.GetBoolValue("FrameGeneration","Enabled",true),
        "saving a staged ordinary presenter writes its off default, not the live owner request");
    Require(generation.RuntimeInterpolationRequested(),"saving defaults cannot toggle the current presenter");
    generation.RequestRuntimeInterpolation(false);generation.RequestRuntimeInterpolation(true);
    generation.StoreInterpolationPreference(defaults);
    Require(defaults.GetBoolValue("FrameGeneration","Enabled",false),"explicit live toggles update the default preference");
    for(long backend:{0L,1L,2L}) {
        CSimpleIniA missing;missing.SetLongValue("FrameGeneration","Backend",backend);
        generation.LoadStartupPreferences(missing);
        Require(generation.RuntimeInterpolationRequested()==(backend!=0),"missing Enabled defaults off only for ordinary presenter");
        missing.SetBoolValue("FrameGeneration","Enabled",true);generation.LoadStartupPreferences(missing);
        Require(generation.RuntimeInterpolationRequested(),"explicit startup interpolation request remains authoritative");
    }
}
void LiveGenerationWithOrdinaryDraft()
{
    auto& generation=*SourceFrameGeneration::GetSingleton();generation.RequestRuntimeInterpolation(false);
    generation.settings.generationBackend=0;
    RendererSettingsDraft draft;draft.valid=true;draft.upscaleType=FSR;draft.generationBackend=0;draft.generationEnabled=false;
    draft.fsr.sourceColorEncoding=Upscaling::ColorEncoding::Gamma22;
    SetLiveGenerationRequest(draft,generation,true,1);
    Require(generation.RuntimeInterpolationRequested(),"explicit checkbox still enables the current FG owner with ordinary staged");
    Require(!draft.generationEnabled&&!generation.settings.enabled,
        "live checkbox must retain staged ordinary startup off preference");
    RendererSettingsCapabilities caps{true,false,true,false};caps.fsrBuilt=true;
    Require(!ValidateRendererSettings(draft,caps),"live checkbox cannot make a staged ordinary draft invalid");
    CSimpleIniA defaults;generation.StoreInterpolationPreference(defaults);
    Require(!defaults.GetBoolValue("FrameGeneration","Enabled",true),"save after live checkbox keeps ordinary startup valid");
    // A menu draft that has not been applied cannot overwrite the current owner's default.
    generation.settings.generationBackend=1;generation.RequestRuntimeInterpolation(false);
    SetLiveGenerationRequest(draft,generation,true,1);
    Require(generation.RuntimeInterpolationRequested()&&generation.settings.enabled&&!draft.generationEnabled,
        "unapplied ordinary draft leaves explicit live action in the current NVIDIA default");
    generation.StoreInterpolationPreference(defaults);
    Require(defaults.GetBoolValue("FrameGeneration","Enabled",false),"save outside draft Apply persists explicit current-owner toggle");
    RendererSettingsDraft roundtrip;roundtrip.valid=true;roundtrip.generationEnabled=true;
    SetRendererUpscaleMode(roundtrip,FSR);SetLiveGenerationRequest(roundtrip,generation,false,1);
    SetRendererUpscaleMode(roundtrip,DLSS);
    Require(!roundtrip.generationEnabled,"provider round trip cannot undo an intentional live FG checkbox action");
}
void ActualPresenterGenerationGate()
{
    struct Case {long active,pending;bool initial,requested,expected;};
    const Case cases[]{
        {1,0,true,false,true},{1,2,true,false,true},{1,1,true,false,false},
        {1,1,false,true,true},{2,0,true,false,true},{2,1,true,false,true},
        {2,2,true,false,false},{2,2,false,true,true},{0,2,false,true,false},
        {0,1,false,true,false},{0,0,false,false,false}};
    auto& generation=*SourceFrameGeneration::GetSingleton();
    for(const auto& c:cases) {
        generation.RequestRuntimeInterpolation(c.initial);generation.settings.generationBackend=c.active;
        RendererSettingsDraft draft;draft.generationBackend=c.pending;draft.generationEnabled=c.requested;
        for(int apply=0;apply<2;++apply) {
            ApplyRendererGeneration(draft,generation,c.active);
            Require(generation.RuntimeInterpolationRequested()==c.expected,
                "Apply uses actual NVIDIA/AMD/ordinary owner even after pending backend has been stored");
            Require(generation.settings.enabled==c.requested,"Apply preserves requested next-launch default separately");
        }
        CSimpleIniA defaults;generation.StoreInterpolationPreference(defaults);
        Require(defaults.GetBoolValue("FrameGeneration","Enabled",!c.requested)==c.requested,
            "actual presenter gate saves pending interpolation preference");
    }
}
void EffectivePresenterUi()
{
    RendererSettingsCapabilities caps{true,false,true,false};
    caps.fsrBuilt=true;caps.fsrFgBuilt=true;caps.fsrFgPresenter=true;
    for(const int nextMode : {DLSS,DLAA,FSR}) {
        RendererSettingsDraft draft;draft.valid=true;draft.upscaleType=nextMode;
        draft.generationBackend=nextMode==FSR?0:1;draft.generationEnabled=nextMode!=FSR;
        draft.fsr.sourceColorEncoding=TheosRenderPipeline::Upscaling::ColorEncoding::Gamma22;
        Require(!ValidateRendererSettings(draft,caps),"pending presenter remains valid with current native UI retained");
        draft.nativeUI=false;
        Require(ValidateRendererSettings(draft,caps)!=nullptr,
            "EffectiveAmdPresenterRequiresNativeUiEvenWhenPendingPresenterChanges");
        auto inactive=caps;inactive.fsrFgPresenter=false;
        Require(!ValidateRendererSettings(draft,inactive),"ordinary/NVIDIA presenter retains native UI opt-out");
    }
}
}
int main()
{
    try { ImmediateMenuEdits(); NativeRenderScale(); Feedback(); Generation(); Neural(); LiveGenerationActions(); DraftModeRoundTrip(); StagedGenerationDefaults(); ActualPresenterGenerationGate(); LiveGenerationWithOrdinaryDraft(); EffectivePresenterUi(); CommunityNeural(); std::cout << "PASS: automatic menu edits, Native render scale, visible/logged rejection, live FG Apply/Save, effective presenter UI, generation round trips and NR capability loss\n"; return 0; }
    catch (const std::exception& error) { std::cerr << error.what() << '\n'; return 1; }
}
