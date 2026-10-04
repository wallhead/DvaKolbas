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
void Generation()
{
    using namespace SourceDLSSG;
    for (const unsigned count : {1u, 3u, 5u}) for (const bool dynamic : {false, true}) {
        for (const unsigned target : {0u, 1u, 60u, 61u, 165u, 1000u, 1001u}) {
            GenerationRequest request{count, target, dynamic};
            Require(ValidGenerationRequest(request) == (!dynamic || target == 0 || (target >= 61 && target <= 1000)),
                "only active dynamic targets block validation");
            Preferences preferences; preferences.generation = request;
            CSimpleIniA ini; StorePreferences(ini, preferences);
            ini.SetLongValue("SourceDLSSG", "DynamicTargetFPS", target); // Also cover manually edited invalid INIs.
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
        StorePreferences(ini, preferences);
        Require(!LoadPreferences(ini).uiRecomposition, "a saved opt-out round trips");
        RendererSettingsDraft current; current.valid = true;
        auto draft = current; draft.sourceDLSSG.uiRecomposition = false;
        Require(CountRendererSettingsChanges(draft, current) == 1, "UI recomposition participates in Apply/Discard");
    }
}
void Neural()
{
    RendererSettingsDraft current; current.valid = true; current.sourceDLSSG.neuralEnabled = true;
    auto draft = current; draft.upscaleType = DLAA; draft.qualityLevel = 4;
    RendererSettingsCapabilities lostUI{true,true,false,false};
    Require(ValidateRendererSettings(draft, lostUI) != nullptr, "new unavailable NR requests rejected");
    Require(ValidateRendererSettings(draft, lostUI, &current) == nullptr, "unchanged NR cannot block unrelated DLAA save");
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
    Require(ValidateRendererSettings(after,caps),"reduced-resolution After remains unavailable");
    after.fsr.quality=Upscaling::Quality::NativeAA;
    Require(!ValidateRendererSettings(after,caps),"Native AA permits source NR after FSR before FG");
    after.upscaleType=DLAA;after.generationBackend=1;
    Require(!ValidateRendererSettings(after,caps),"DLAA permits source NR after reconstruction");
    after.upscaleType=DLSS;
    Require(ValidateRendererSettings(after,caps),"scaled DLSS After cannot claim qualified display guides");
    auto twice=draft; twice.sourceDLSSG.neuralPasses=2;
    Require(ValidateRendererSettings(twice,caps),"first community trial permits one pass");
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
        SetLiveGenerationRequest(draft,generation,selected);
        Require(generation.RuntimeInterpolationRequested()==selected,"checkbox takes effect immediately");
        // An unrelated edit followed by Apply/Save must preserve the checkbox.
        draft.autoExposure=false;
        RendererSettingsCapabilities caps{true,false,true,false};caps.fsrBuilt=true;caps.fsrFgBuilt=true;
        Require(!ValidateRendererSettings(draft,caps),"NVIDIA/AMD draft remains valid");
        ApplyRendererGeneration(draft,generation);
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
    try { Feedback(); Generation(); Neural(); LiveGenerationActions(); EffectivePresenterUi(); CommunityNeural(); std::cout << "PASS: visible/logged rejection, live FG Apply/Save, effective presenter UI, generation round trips and NR capability loss\n"; return 0; }
    catch (const std::exception& error) { std::cerr << error.what() << '\n'; return 1; }
}
