#pragma once
#include <condition_variable>
#include <cstdint>
#include <filesystem>
#include <mutex>
#include <thread>

namespace TheosRenderPipeline {
enum class AudioState { Stopped, Starting, Playing, Stopping, Error };
struct AudioSnapshot {
  AudioState state{AudioState::Stopped};
  std::int32_t error{};
};
class AudioPlayer {
public:
  explicit AudioPlayer(std::filesystem::path track, double volume = 1.0);
  ~AudioPlayer();
  void Play() noexcept;
  void Stop() noexcept;
  AudioSnapshot Snapshot() const;

private:
  void Submit(bool play) noexcept;
  void Run(std::stop_token stop) noexcept;
  void Publish(std::uint64_t request, AudioSnapshot value);
  std::filesystem::path track_;
  double volume_;
  mutable std::mutex mutex_;
  std::condition_variable condition_;
  AudioSnapshot status_;
  std::uint64_t request_{};
  bool play_{};
  std::jthread worker_;
};
} // namespace TheosRenderPipeline
