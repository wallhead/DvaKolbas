#include "CommunityShaderIntegration.h"
#include "DLSSPreset.h"
#include "FrameGen/NvidiaHost.h"
#include "FrameGen/SourceDLSSGBackend.h"
#include "OverlayFrameView.h"
#include "OverlayFsrGenerationControls.h"
#include "OverlayUI.h"
#include "OverlayUIStyle.h"
#include "RenderPipeline.h"
#include "VideoMemoryTelemetry.h"
#include <PCH.h>

using namespace TheosRenderPipeline::Overlay;

namespace
{
bool DrawTextureCapCombo(const char* a_label, std::uint32_t& a_cap)
{
    static constexpr const char* kNames[]{"Full size", "512", "1024", "2048", "4096"};
    static constexpr std::uint32_t kValues[]{0, 512, 1024, 2048, 4096};
    int selected = 0;
    for (int i = 0; i < static_cast<int>(std::size(kValues)); ++i)
    {
        if (a_cap == kValues[i])
        {
            selected = i;
            break;
        }
    }
    ImGui::SetNextItemWidth(-1.0f);
    if (!ImGui::Combo(a_label, &selected, kNames, static_cast<int>(std::size(kNames))))
    {
        return false;
    }
    a_cap = kValues[selected];
    return true;
}

int TexturePresetIndex(const TextureProviderBridge::Settings& a_settings)
{
    const auto allEqual = [&](std::uint32_t a_value) {
        return std::ranges::all_of(a_settings.maxSize, [=](std::uint32_t a_cap) { return a_cap == a_value; });
    };
    if (allEqual(2048))
    {
        return 0;
    }
    if (allEqual(1024))
    {
        return 1;
    }
    if (a_settings.maxSize[0] == 1024 && std::ranges::all_of(a_settings.maxSize.begin() + 1, a_settings.maxSize.end(),
                                                             [](std::uint32_t a_cap) { return a_cap == 512; }))
    {
        return 2;
    }
    return 3;
}

void ApplyTexturePreset(TextureProviderBridge::Settings& a_settings, int a_preset)
{
    if (a_preset == 0 || a_preset == 1)
    {
        a_settings.maxSize.fill(a_preset == 0 ? 2048u : 1024u);
    }
    else if (a_preset == 2)
    {
        a_settings.maxSize.fill(512);
        a_settings.maxSize[0] = 1024;
    }
}
} // namespace

void OverlayUI::DrawHDROutputSettings()
{
    if(NvidiaHost::GetSingleton()->FsrActive() || settingsDraft.upscaleType==FSR) {
        ImGui::TextWrapped("HDR output is unavailable with FSR.");return;
    }
    namespace HDR = TheosRenderPipeline::HDROutput;
    auto& hdr = settingsDraft.sourceDLSSG.hdrOutput;
    const auto state = TheosRenderPipeline::SourceDLSSG::Backend::Get().HDRState();
    DrawSettingsHeading("HDR output (experimental)", "on/off: save and restart");
    ImGui::Checkbox("HDR output", &hdr.enabled);
    DrawSettingsHelp("Expands the finished SDR image (including ENB) to HDR10 and shows the UI at its own "
                     "brightness. Highlights the preset already clipped cannot be recovered. Requires Windows HDR.");
    if (hdr.enabled != state.requested)
    {
        ImGui::TextWrapped("Save as default and restart to %s HDR output", hdr.enabled ? "allocate" : "release");
    }
    else if (state.requested)
    {
        const auto status = state.displayMaxNits > 0.0f ?
            std::format("{} (display reports {:.0f} nits)", state.reason, state.displayMaxNits) : std::string(state.reason);
        if (state.display) { ImGui::TextDisabled("%s", status.c_str()); }
        else { ImGui::TextWrapped("%s", status.c_str()); }
        if (state.gpu.samples)
        {
            ImGui::TextDisabled("Output pass GPU: %.2f ms average, %.2f ms max", state.gpu.AverageUs() / 1000.0,
                                state.gpu.maxUs / 1000.0);
        }
    }
    ImGui::BeginDisabled(!hdr.enabled);
    ImGui::Checkbox("Match Windows SDR brightness##hdr", &hdr.matchWindowsSDR);
    const bool windowsKnown = state.windowsSDRWhiteNits >= HDR::kMinimumNits;
    if (hdr.matchWindowsSDR)
    {
        if (windowsKnown) { ImGui::TextDisabled("Paper white and UI: %.0f nits from Windows", state.windowsSDRWhiteNits); }
        else if (state.requested) { ImGui::TextWrapped("Windows SDR brightness unavailable; using the values below"); }
    }
    DrawSettingsHelp("Uses Windows' SDR content brightness (Settings > Display > HDR) for paper white and UI, "
                     "so whites match the desktop. Untick to set them here.");
    const bool followsWindows = hdr.matchWindowsSDR && windowsKnown;
    // While following Windows, show the values in use; the manual values stay saved for unticking.
    auto effective = HDR::Effective(hdr, state.windowsSDRWhiteNits);
    ImGui::BeginDisabled(followsWindows);
    ImGui::SliderFloat("Paper white##hdr", followsWindows ? &effective.paperWhiteNits : &hdr.paperWhiteNits,
                       HDR::kMinimumNits, 500.0f, "%.0f nits");
    DrawSettingsHelp("Brightness of SDR white in the scene. Start near the Windows SDR content brightness.");
    ImGui::EndDisabled();
    ImGui::SliderFloat("Peak brightness##hdr", &hdr.peakNits, effective.paperWhiteNits, 4000.0f, "%.0f nits");
    if (state.displayMaxNits > 0.0f && ImGui::SmallButton("Use display peak##hdr"))
    {
        hdr.peakNits = state.displayMaxNits;
    }
    DrawSettingsHelp("Brightest expanded highlight. Set to your display's peak.");
    ImGui::BeginDisabled(followsWindows);
    ImGui::SliderFloat("UI brightness##hdr", followsWindows ? &effective.uiNits : &hdr.uiNits,
                       HDR::kMinimumNits, 500.0f, "%.0f nits");
    ImGui::EndDisabled();
    ImGui::SliderFloat("Highlight strength##hdr", &hdr.highlightStrength, 0.0f, 1.0f, "%.2f");
    DrawSettingsHelp("0 keeps the SDR range at paper white; 1 expands the brightest pixels to peak brightness.");
    ImGui::SliderFloat("Expansion start##hdr", &hdr.expansionStart, 0.1f, 0.95f, "%.2f");
    DrawSettingsHelp("SDR brightness where expansion begins. Higher values boost only the brightest areas.");
    auto& backend = TheosRenderPipeline::SourceDLSSG::Backend::Get();
    bool capture = backend.HDRDiagnosticCapture();
    if (ImGui::Checkbox("Capture HDR samples##hdr", &capture)) { backend.ConfigureHDRDiagnosticCapture(capture); }
    DrawSettingsHelp("Temporary HDR-only measurements, once every two seconds. Other debug traces remain unchanged. Resets on restart; small sampling overhead is possible.");
    if (capture)
    {
        bool patches = backend.HDRCalibrationPattern();
        if (ImGui::Checkbox("HDR calibration patches##hdr", &patches)) { backend.ConfigureHDRCalibrationPattern(patches); }
        const auto levels = TheosRenderPipeline::HDROutput::CalibrationLevels(state.displayMaxNits);
        ImGui::TextWrapped("Patches, left to right: %.0f, %.0f, %.0f, %.0f nits", levels[0], levels[1], levels[2], levels[3]);
        DrawSettingsHelp("Top-left reference patches bypass highlight expansion and use opaque foreground tags for FG. The last two bracket 80%/120% of the reported display peak; unknown displays use 500/1000 nits. Patch measurements are logged separately and excluded from scene statistics. Resets on restart.");
    }
    const char* transfers[]{"Gamma 2.2", "sRGB"};
    int transfer = static_cast<int>(hdr.transfer);
    ImGui::SetNextItemWidth(-1);
    if (ImGui::Combo("##hdrTransfer", &transfer, transfers, 2))
    {
        hdr.transfer = static_cast<HDR::Transfer>(transfer);
    }
    DrawSettingsHelp("How the SDR image is decoded. Gamma 2.2 matches most SDR monitors; sRGB lifts shadows.");
    ImGui::EndDisabled();
    hdr = HDR::Sanitize(hdr);
}

void OverlayUI::DrawImagePanel(float tabCardHeight, const FrameView& view)
{
    auto* host = NvidiaHost::GetSingleton();
    if (!ImGui::BeginTabItem("DLSS", nullptr,
                             requestedPage == SettingsPage::Image ? ImGuiTabItemFlags_SetSelected : 0))
    {
        return;
    }
    if (!BeginSettingsColumns("image", tabCardHeight, view))
    {
        ImGui::EndTabItem();
        return;
    }
    if (ImGui::CollapsingHeader("Status and measurements"))
        DrawImageMeasurements(view);
    NextSettingsColumn(tabCardHeight);
    if (TheosRenderPipeline::CommunityShaders::Active())
    {
        DrawSettingsHeading("Controlled by Community Shaders", "");
        ImGui::TextWrapped("Community Shaders controls upscaling, render scale, model preset, sharpening and camera "
                           "jitter. Change those settings in its menu.");
        DrawSettingsHelp(
            "TRP's Neural Rendering, frame generation and Reflex controls remain available in their own tabs.");
        ImGui::Separator();
        DrawSettingsHeading("HDR", "");
        ImGui::TextWrapped("Use Community Shaders' HDR Display for HDR with Community Shaders.");
    }
    else
    {
        ImGui::TextDisabled("Mode / scale: save and restart");
        if (ImGui::BeginTable("##resolution", 2, ImGuiTableFlags_SizingStretchSame))
        {
            ImGui::TableNextColumn();
            ImGui::TextUnformatted("Mode");
            const bool restricted=RenderPipeline::GetSingleton()->mFsrOnlyRenderer || TheosRenderPipeline::IsAmdRenderer(RenderPipeline::GetSingleton()->mAdapterVendorId);
            const auto modeLabel=settingsDraft.upscaleType==Xess?"XeSS":settingsDraft.upscaleType==FSR?"FSR":"DLSS";
            ImGui::SetNextItemWidth(-1);
            if(ImGui::BeginCombo("##mode",modeLabel)) {
                const auto choose=[&](const char* label,int mode){
                    if(ImGui::Selectable(label,settingsDraft.upscaleType==mode)) {
                        const bool neural=settingsDraft.sourceDLSSG.neuralEnabled,generation=settingsDraft.generationEnabled;
                        TheosRenderPipeline::SetRendererUpscaleMode(settingsDraft,mode);
                        settingsDraft.sourceDLSSG.neuralEnabled=neural;settingsDraft.generationEnabled=generation;
                    }
                };
                if(!restricted)choose("DLSS",settingsDraft.nvidiaMode.nativeScale?DLAA:DLSS);
#if defined(TRP_ENABLE_FSR)
                choose("FSR",FSR);
#endif
#if defined(TRP_ENABLE_XESS)
                choose("XeSS",Xess);
#endif
                ImGui::EndCombo();
            }
            ImGui::TableNextColumn();
            ImGui::TextUnformatted("Render scale");
            if(settingsDraft.upscaleType==Xess) {
                const char* qualities[]{"Quality","Balanced","Performance","100% | Native"};
                int quality=static_cast<int>(settingsDraft.xess.quality);
                ImGui::SetNextItemWidth(-1);
                if(ImGui::Combo("##xessQuality",&quality,qualities,4))settingsDraft.xess.quality=static_cast<TheosRenderPipeline::Upscaling::Quality>(quality);
            } else if(settingsDraft.upscaleType==FSR) {
                const char* qualities[]{"67% | Quality","59% | Balanced","50% | Performance","100% | Native"};
                int quality=static_cast<int>(settingsDraft.fsr.quality);
                ImGui::SetNextItemWidth(-1);
                if(ImGui::Combo("##fsrQuality",&quality,qualities,4))settingsDraft.fsr.quality=static_cast<TheosRenderPipeline::Upscaling::Quality>(quality);
            } else {
            const char* scales[]{"50% | Performance", "58% | Balanced", "67% | Quality", "33% | Ultra Performance",
                                 "78% | Ultra Quality"};
            const bool native = settingsDraft.upscaleType == DLAA;
            ImGui::SetNextItemWidth(-1);
            if (ImGui::BeginCombo("##renderScale",
                                  native ? "100% | Native" : scales[std::clamp(settingsDraft.qualityLevel, 0, 4)]))
            {
                for (const int i : {3, 0, 1, 2, 4})
                {
                    if (ImGui::Selectable(scales[i], !native && settingsDraft.qualityLevel == i))
                    {
                        TheosRenderPipeline::SetNvidiaRenderScale(settingsDraft, i);
                    }
                }
                if (ImGui::Selectable("100% | Native", native))
                    TheosRenderPipeline::SetNvidiaRenderScale(settingsDraft, -1);
                ImGui::EndCombo();
            }
            }
            ImGui::EndTable();
        }
        DrawSettingsHelp(
            "Native uses the full output resolution. Lower render scales reduce the size of the rendered world.");
        if (view.sourceDLSSGActive || view.fsrActive || host->XessActive())
        {
            const auto& configuration = host->SourceUpscalerSettings();
            if (configuration.NeedsRestart())
            {
                ImGui::TextWrapped("Mode/render scale awaiting restart");
            }
            if (configuration.Failed())
            {
                ImGui::TextWrapped("Feature update failed; restart required");
            }
            else if (configuration.NeedsLiveChange())
            {
                ImGui::TextDisabled("Feature update queued");
            }
        }
        else
        {
            ImGui::TextWrapped("Presentation host is unavailable.");
        }
        if(view.fsrActive || settingsDraft.upscaleType==FSR) {
            ImGui::TextWrapped("%s",view.fsrStatus.text.c_str());
            DrawSettingsHelp("Choosing FSR stages the next launch; current NR and FG stay active. "
                             "Save and restart to change mode, quality, provider or source color encoding. "
                             "FSR keeps its FG backend ready for live on/off and saves HDR and dynamic resolution off. "
                             "After NR supports fixed DLSS/FSR render scales. Sharpness applies after editing ends.");
        }
        if(settingsDraft.upscaleType==Xess) {
            const auto status=host->XessStatus();ImGui::TextWrapped("%s",status.text.c_str());
            const char* encodings[]{"Linear","Gamma 2.2 SDR","sRGB SDR"};
            int encoding=static_cast<int>(settingsDraft.xess.sourceEncoding)-1;ImGui::SetNextItemWidth(-1);
            if(ImGui::Combo("Source color encoding##xess",&encoding,encodings,3))settingsDraft.xess.sourceEncoding=static_cast<TheosRenderPipeline::Upscaling::ColorEncoding>(encoding+1);
            ImGui::BeginDisabled(host->XessActive() && !host->XessSharpeningAvailable());
            ImGui::SliderFloat("Sharpness##xess",&settingsDraft.xess.sharpness,0,1,"%.2f");
            ImGui::EndDisabled();
            DrawSettingsHelp("0 = off, 1 = maximum. RCAS sharpens after XeSS, ReShade after-upscaling effects and NR, before FG and UI. This also sharpens ReShade grain/noise. Applies on slider release without restart; Save as default keeps it.");
            if(host->XessActive()) {
                DrawSettingsValue("Active XeSS sharpness",std::format("{:.2f}",host->XessAppliedSharpness()).c_str());
                const auto warning=host->XessSharpeningStatus();if(!warning.empty())ImGui::TextWrapped("%s",warning.c_str());
            }
            DrawSettingsHelp("Experimental XeSS SR: tested on RTX 4080 SUPER; AMD, Intel and GTX hardware qualification is pending. SDK-sized fixed resolution and linear FP16 input. Community NR is available on eligible NVIDIA GPUs. FSR FG requires selecting its backend and restarting; NVIDIA FG/HDR remain pending. Save and restart for quality/encoding changes. For NR, match [NeuralRendering Advanced] SourceColorEncoding to [XeSS].");
        } else if(settingsDraft.upscaleType==FSR) {
            const char* policies[]{"FSR3 (3.1.5)","Auto (FSR4 / FSR3)","FSR4 (ML)"};
            int policy=static_cast<int>(settingsDraft.fsr.providerPolicy);
            const auto availability=host->FsrMlChoices();
            if(DrawFsrProviderChoice("Provider##fsr",policy,policies,availability.upscale.value_or(true),availability.upscaleReason.c_str(),availability.analyticalOnly))
                settingsDraft.fsr.providerPolicy=static_cast<TheosRenderPipeline::Upscaling::ProviderPolicy>(policy);
            DrawSettingsHelp("Save and restart after changing provider. FSR3 keeps the official 3.1.5 runtime. FSR4 uses the separate INT8 runtime on NVIDIA (SM6.6 required), or official ML on supported AMD hardware. Auto uses the official runtime and may select FSR3. The status shows the actual provider. Frame generation is selected independently.");
            const char* encodings[]{"Unknown (choose before enabling FSR)","Linear SDR","Gamma 2.2 SDR","sRGB SDR"};
            int encoding=static_cast<int>(settingsDraft.fsr.sourceColorEncoding);
            if(ImGui::Combo("Source color encoding##fsr",&encoding,encodings,4))settingsDraft.fsr.sourceColorEncoding=static_cast<TheosRenderPipeline::Upscaling::ColorEncoding>(encoding);
            DrawSettingsHelp("Choose the actual Skyrim/ENB source encoding. Texture format does not determine it. Unknown prevents FSR startup; changing encoding requires Save and restart.");
            ImGui::SliderFloat("Sharpness##fsr",&settingsDraft.fsr.sharpness,0,1,"%.2f");
            DrawSettingsHelp("0 = off, 1 = maximum. Applies when you release the slider, without restarting. The NR tab's DLSS sharpening does not affect FSR.");
            const auto* sharpnessHost=NvidiaHost::GetSingleton();
            if(sharpnessHost->FsrActive())
                DrawSettingsValue("Active FSR sharpness",std::format("{:.2f}",sharpnessHost->SourceUpscalerSettings().Effective().fsr.sharpness).c_str());
            if (TheosRenderPipeline::CommunityShaders::Active())
                ImGui::TextWrapped("Sharpness inactive: Community Shaders owns upscaling.");
            else if (!sharpnessHost->FsrActive())
                ImGui::TextWrapped("Sharpness inactive: FSR awaits save and restart.");
            else if (!sharpnessHost->FsrTemporalActive())
                ImGui::TextWrapped("Sharpness inactive: awaiting a temporal FSR frame. Spatial recovery does not sharpen.");
            ImGui::TextWrapped("Reactive and transparency masks are unavailable. Auto exposure is enabled. Camera jitter uses the selected provider.");
        } else {
        ImGui::Separator();
        ImGui::TextUnformatted("DLSS model preset");
        ImGui::SetNextItemWidth(-1);
        if (ImGui::BeginCombo("##dlssPreset", TheosRenderPipeline::DLSSPreset::Label(settingsDraft.dlssPreset)))
        {
            for (const auto& entry : TheosRenderPipeline::DLSSPreset::entries)
            {
                if (ImGui::Selectable(entry.label, entry.value == settingsDraft.dlssPreset))
                {
                    settingsDraft.dlssPreset = entry.value;
                }
            }
            ImGui::EndCombo();
        }
        DrawSettingsHelp(
            std::format("{}\nRequested preset; NVIDIA chooses the actual model. L/M require a supporting runtime.",
                        TheosRenderPipeline::DLSSPreset::Description(settingsDraft.dlssPreset))
                .c_str());
        ImGui::Spacing();
#if defined(TRP_NO_NEURAL_RENDERING)
        // Without NR there is no Neural Rendering tab to hold sharpening.
        ImGui::Checkbox("Sharpening", &settingsDraft.sharpening);
        ImGui::BeginDisabled(!settingsDraft.sharpening);
        ImGui::SliderFloat("Strength##sharpness", &settingsDraft.sharpness, 0, 1, "%.2f");
        ImGui::EndDisabled();
#else
        DrawSettingsValue("Sharpening", settingsDraft.sharpening ? std::format("{:.2f}", settingsDraft.sharpness).c_str() : "Off");
        DrawSettingsHelp("Set in the Neural Rendering tab. Applies with or without NR.");
#endif
        DrawPresetSharpeningStatus();
        ImGui::Separator();

        ImGui::Checkbox("Auto exposure", &settingsDraft.autoExposure);
        ImGui::Checkbox("Camera jitter", &settingsDraft.enableJitter);
        ImGui::Separator();
        DrawHDROutputSettings();
        }

        if (showDeveloperControls)
        {
            DrawOutputOptimizations();
        }
    }
    if (view.textureProviderAvailable)
    {
        DrawTextureMemoryPanel(view);
    }
    DrawAdvancedPanel(tabCardHeight, view);
    EndSettingsColumns();
    ImGui::EndTabItem();
}

void OverlayUI::DrawTextureMemoryPanel(const FrameView& view)
{
    ImGui::Spacing();
    ImGui::SeparatorText("Texture memory");

    ImGui::TextDisabled("Applies to future texture loads");
    ImGui::BeginDisabled(!view.textureProviderAvailable);
    ImGui::Checkbox("Enable runtime mip caps", &settingsDraft.textureProviderSettings.enabled);
    ImGui::TextDisabled("Preset");
    const char* texturePresetNames[]{"Quality | 2048", "Balanced | 1024", "Performance | 1024 / 512", "Custom"};
    int texturePreset = TexturePresetIndex(settingsDraft.textureProviderSettings);
    ImGui::SetNextItemWidth(-1.0f);
    if (ImGui::Combo("##texturePreset", &texturePreset, texturePresetNames,
                     static_cast<int>(std::size(texturePresetNames))))
    {
        ApplyTexturePreset(settingsDraft.textureProviderSettings, texturePreset);
    }
    ImGui::Spacing();
    static constexpr const char* kTextureCategories[]{"Diffuse", "Normal", "Parallax", "Material", "Glow", "Mask"};
    if (ImGui::BeginTable("##textureCaps", 2, ImGuiTableFlags_SizingStretchProp))
    {
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 105.0f);
        ImGui::TableSetupColumn("Maximum dimension", ImGuiTableColumnFlags_WidthStretch, 1.0f);
        for (std::size_t i = 0; i < settingsDraft.textureProviderSettings.maxSize.size(); ++i)
        {
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(kTextureCategories[i]);
            ImGui::TableNextColumn();
            ImGui::PushID(static_cast<int>(i));
            DrawTextureCapCombo("##textureCap", settingsDraft.textureProviderSettings.maxSize[i]);
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
    ImGui::EndDisabled();
    ImGui::Spacing();
    if (view.textureProviderAvailable && settingsDraft.textureProviderSettings.enabled &&
        !view.textureTelemetry.hooksInstalled)
    {
        ImGui::TextWrapped("The provider started without hooks. Save the startup default and relaunch "
                                   "to enable texture interception.");
    }
    else
    {
        ImGui::TextDisabled("Folder rules: TextureDownscaler menu");
        DrawSettingsHelp("Existing textures stay unchanged until reloaded. Folder rules and exclusions are in "
                         "TextureDownscaler's Menu Framework page.");
    }
}
