#include "RazkolbasAudio.h"
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <thread>

using namespace TheosRenderPipeline;
using namespace std::chrono_literals;
void Require(bool value, const char *message) {
  if (!value)
    throw std::runtime_error(message);
}
bool Wait(AudioPlayer &player, AudioState state,
          std::chrono::seconds timeout = 10s) {
  const auto deadline = std::chrono::steady_clock::now() + timeout;
  do {
    if (player.Snapshot().state == state)
      return true;
    std::this_thread::sleep_for(20ms);
  } while (std::chrono::steady_clock::now() < deadline);
  return false;
}
int main(int argc, char **argv) {
  try {
    Require(argc == 2, "supply the packaged MP3");
    AudioPlayer missing(
        std::filesystem::path(argv[1]).parent_path() / "absent.mp3", 0);
    Require(missing.Snapshot().state == AudioState::Stopped, "never autoplay");
    const auto start = std::chrono::steady_clock::now();
    missing.Play();
    Require(std::chrono::steady_clock::now() - start < 250ms,
            "Play must not wait for decoding");
    Require(Wait(missing, AudioState::Error),
            "missing track must report an error");
    Require(missing.Snapshot().error < 0, "failure has an HRESULT");
    missing.Stop();
    Require(Wait(missing, AudioState::Stopped),
            "Stop clears a failed playback request");

    const auto invalidPath =
        std::filesystem::temp_directory_path() /
        ("razkolbas-invalid-" +
         std::to_string(
             std::chrono::steady_clock::now().time_since_epoch().count()) +
         ".mp3");
    struct RemoveFile {
      std::filesystem::path path;
      ~RemoveFile() {
        std::error_code error;
        std::filesystem::remove(path, error);
      }
    } removeInvalid{invalidPath};
    {
      std::ofstream file(invalidPath, std::ios::binary);
      file << "not an MP3";
    }
    AudioPlayer invalid(invalidPath, 0);
    invalid.Play();
    Require(Wait(invalid, AudioState::Error),
            "decoder failures must reach the UI status");

    // Decode/play the actual user-supplied asset without changing system
    // volume.
    AudioPlayer player(std::filesystem::path(argv[1]), 0);
    Require(player.Snapshot().state == AudioState::Stopped,
            "valid track also starts silent");
    player.Play();
    Require(Wait(player, AudioState::Playing),
            "actual MP3 must decode and play");
    player.Stop();
    Require(Wait(player, AudioState::Stopped),
            "Stop must stop and reset playback");
    player.Play();
    Require(Wait(player, AudioState::Playing), "Play must work after Stop");
    for (int i = 0; i != 20; ++i) {
      player.Play();
      player.Stop();
    }
    Require(Wait(player, AudioState::Stopped), "last Stop wins rapid commands");
    player.Play();
    player.Stop();
    player.Play();
    Require(Wait(player, AudioState::Playing), "last Play wins rapid commands");
    Require(Wait(player, AudioState::Stopped, 35s),
            "natural track end must reset status and release the player");
    player.Play();
    Require(Wait(player, AudioState::Playing),
            "Play must also work after natural completion");
    // Destructor must also retire an active player safely in the test host.
    std::cout << "PASS: no autoplay, nonblocking commands, missing file, "
                 "actual MP3, reset and rapid toggles\n";
    return 0;
  } catch (const std::exception &error) {
    std::cerr << error.what() << '\n';
    return 1;
  }
}
