#include "OverlayUI.h"
#include "PluginPaths.h"
#include "RazkolbasAudio.h"
#include <imgui.h>

void OverlayUI::DrawRazkolbasPanel() {
  if (!ImGui::BeginTabItem("razkolbas"))
    return;
  try {
    // SKSE keeps the plugin resident for the game process. Retain this
    // optional worker until process exit: never join it under DllMain's
    // loader lock. A standalone owner's destructor still stops/joins.
    if (!razkolbasPlayer)
      razkolbasPlayer = new TheosRenderPipeline::AudioPlayer(
          TheosRenderPipeline::PluginPaths::Directory() /
          L"RaZkolbaS" / L"Audio" / L"razkolbas.mp3");
    auto *player = razkolbasPlayer;
    ImGui::TextWrapped("MC Vspishkin & Nikiforovna - Kolbasny tsekh");
    ImGui::Spacing();
    if (ImGui::Button("Play", ImVec2(90, 0)))
      player->Play();
    ImGui::SameLine();
    if (ImGui::Button("Stop", ImVec2(90, 0)))
      player->Stop();
    const auto status = player->Snapshot();
    float volume = static_cast<float>(status.volume * 100.0);
    ImGui::SetNextItemWidth(-ImGui::CalcTextSize("Volume").x -
                            ImGui::GetStyle().ItemInnerSpacing.x);
    if (ImGui::SliderFloat("Volume", &volume, 0, 100, "%.0f%%",
                           ImGuiSliderFlags_AlwaysClamp))
      player->SetVolume(volume / 100.0);
    using TheosRenderPipeline::AudioState;
    switch (status.state) {
    case AudioState::Stopped:
      ImGui::TextDisabled("Stopped");
      break;
    case AudioState::Starting:
      ImGui::TextDisabled("Starting...");
      break;
    case AudioState::Playing:
      ImGui::TextUnformatted("Playing");
      break;
    case AudioState::Stopping:
      ImGui::TextDisabled("Stopping...");
      break;
    case AudioState::Error:
      ImGui::TextWrapped(
          "Playback unavailable (0x%08X). Check Audio/razkolbas.mp3.",
          static_cast<unsigned>(status.error));
      break;
    }
  } catch (...) {
    ImGui::TextWrapped("Audio player unavailable.");
  }
  ImGui::EndTabItem();
}
