#include "CommunityShaderIntegration.h"
#include "DLSSPreset.h"
#include "FrameGen/NvidiaHost.h"
#include "FrameGen/SourceDLSSGBackend.h"
#include "OverlayFrameView.h"
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
        ImGui::TextColored(kAmber, "Save as default and restart to %s HDR output", hdr.enabled ? "allocate" : "release");
    }
    else if (state.requested)
    {
        const auto status = state.displayMaxNits > 0.0f ?
            std::format("{} (display reports {:.0f} nits)", state.reason, state.displayMaxNits) : std::string(state.reason);
        if (state.display) { ImGui::TextDisabled("%s", status.c_str()); }
        else { ImGui::TextColored(kAmber, "%s", status.c_str()); }
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
        else if (state.requested) { ImGui::TextColored(kAmber, "Windows SDR brightness unavailable; using the values below"); }
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
    if (!ImGui::BeginTabItem("Image", nullptr,
                             requestedPage == SettingsPage::Image ? ImGuiTabItemFlags_SetSelected : 0))
    {
        return;
    }
    if (!BeginSettingsColumns("image", tabCardHeight, view))
    {
        ImGui::EndTabItem();
        return;
    }
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
            const char* modes[]{"DLSS", "DLAA", "FSR (SR only)"};
            int mode = settingsDraft.upscaleType == FSR ? 2 : settingsDraft.upscaleType == DLAA ? 1 : 0;
            constexpr int modeCount =
#if defined(TRP_ENABLE_FSR)
                3;
#else
                2;
#endif
            ImGui::SetNextItemWidth(-1);
            if (ImGui::Combo("##mode", &mode, modes, modeCount))
            {
                settingsDraft.upscaleType = mode==2?FSR:mode==1?DLAA:DLSS;
                settingsDraft.generationBackend = mode==2?0:1;
                if(mode==2) {
                    settingsDraft.generationEnabled=false;
                    settingsDraft.sourceDLSSG.neuralEnabled=false;
                    settingsDraft.sourceDLSSG.hdrOutput.enabled=false;
                }
            }
            ImGui::TableNextColumn();
            ImGui::TextUnformatted("Render scale");
            if(settingsDraft.upscaleType==FSR) {
                const char* qualities[]{"67% | Quality","59% | Balanced","50% | Performance","100% | NativeAA"};
                int quality=static_cast<int>(settingsDraft.fsr.quality);
                ImGui::SetNextItemWidth(-1);
                if(ImGui::Combo("##fsrQuality",&quality,qualities,4))settingsDraft.fsr.quality=static_cast<TheosRenderPipeline::Upscaling::Quality>(quality);
            } else {
            const char* scales[]{"50% | Performance", "58% | Balanced", "67% | Quality", "33% | Ultra Performance",
                                 "78% | Ultra Quality"};
            const bool native = settingsDraft.upscaleType == DLAA;
            ImGui::BeginDisabled(native);
            ImGui::SetNextItemWidth(-1);
            if (ImGui::BeginCombo("##renderScale",
                                  native ? "100% | Native" : scales[std::clamp(settingsDraft.qualityLevel, 0, 4)]))
            {
                for (const int i : {3, 0, 1, 2, 4})
                {
                    if (ImGui::Selectable(scales[i], settingsDraft.qualityLevel == i))
                    {
                        settingsDraft.qualityLevel = i;
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::EndDisabled();
            }
            ImGui::EndTable();
        }
        DrawSettingsHelp(
            "DLAA uses native resolution. Lower DLSS render scales reduce the size of the rendered world.");
        if (view.sourceDLSSGActive || view.fsrActive)
        {
            const auto& configuration = host->SourceUpscalerSettings();
            if (configuration.NeedsRestart())
            {
                ImGui::TextColored(kAmber, "Mode/render scale awaiting restart");
            }
            if (configuration.Failed())
            {
                ImGui::TextColored(kRust, "Feature update failed; restart required");
            }
            else if (configuration.NeedsLiveChange())
            {
                ImGui::TextDisabled("Feature update queued");
            }
        }
        else
        {
            ImGui::TextColored(kRust, "Presentation host is unavailable.");
        }
        if(view.fsrActive || settingsDraft.upscaleType==FSR) {
            ImGui::TextWrapped("%s",view.fsrStatus.text.c_str());
            DrawSettingsHelp("FSR (SR only) sets generation off, ordinary backend 0, NR off and HDR off. Save and restart to change mode, quality, provider or source color encoding. Sharpness applies after Present.");
        }
        if(settingsDraft.upscaleType==FSR) {
            const char* policies[]{"Analytical (3.1.5)","Compatible (runtime selected)"};
            int policy=static_cast<int>(settingsDraft.fsr.providerPolicy);
            if(ImGui::Combo("Provider##fsr",&policy,policies,2))settingsDraft.fsr.providerPolicy=static_cast<TheosRenderPipeline::Upscaling::ProviderPolicy>(policy);
            const char* encodings[]{"Unknown (choose before enabling FSR)","Linear SDR","Gamma 2.2 SDR","sRGB SDR"};
            int encoding=static_cast<int>(settingsDraft.fsr.sourceColorEncoding);
            if(ImGui::Combo("Source color encoding##fsr",&encoding,encodings,4))settingsDraft.fsr.sourceColorEncoding=static_cast<TheosRenderPipeline::Upscaling::ColorEncoding>(encoding);
            DrawSettingsHelp("Choose the actual Skyrim/ENB source encoding. Texture format does not determine it. Unknown prevents FSR startup; changing encoding requires Save and restart.");
            ImGui::SliderFloat("Sharpness##fsr",&settingsDraft.fsr.sharpness,0,1,"%.2f");
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
        ImGui::TextColored(kOchre, "The provider started without hooks. Save the startup default and relaunch "
                                   "to enable texture interception.");
    }
    else
    {
        ImGui::TextDisabled("Folder rules: TextureDownscaler menu");
        DrawSettingsHelp("Existing textures stay unchanged until reloaded. Folder rules and exclusions are in "
                         "TextureDownscaler's Menu Framework page.");
    }
}
