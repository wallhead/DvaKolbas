#include "RazkolbasAudio.h"
#include <Windows.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <fstream>
#include <memory>
#include <vector>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Media.Core.h>
#include <winrt/Windows.Media.Playback.h>
#include <winrt/Windows.Storage.Streams.h>

namespace TheosRenderPipeline {
AudioPlayer::AudioPlayer(std::filesystem::path track, double volume)
    : track_(std::move(track)),
      volume_(std::isfinite(volume) ? std::clamp(volume, 0.0, 1.0) : 0.3) {}

AudioPlayer::~AudioPlayer() {
  {
    std::scoped_lock lock(mutex_);
    worker_.request_stop();
  }
  condition_.notify_one();
  // jthread joins before the mutex and status members are destroyed.
}

void AudioPlayer::Play() noexcept { Submit(true); }
void AudioPlayer::Stop() noexcept { Submit(false); }

void AudioPlayer::SetVolume(double volume) {
  std::scoped_lock lock(mutex_);
  const auto value = std::isfinite(volume) ? std::clamp(volume, 0.0, 1.0) : 0.3;
  if (volume_ == value)
    return;
  volume_ = value;
  ++volumeRequest_;
  condition_.notify_one();
}

AudioSnapshot AudioPlayer::Snapshot() const {
  std::scoped_lock lock(mutex_);
  auto value = status_;
  value.volume = volume_;
  return value;
}

void AudioPlayer::Submit(bool play) noexcept {
  try {
    std::scoped_lock lock(mutex_);
    play_ = play;
    ++request_;
    status_ = {play ? AudioState::Starting : AudioState::Stopping, 0};
    if (!worker_.joinable())
      worker_ = std::jthread([this](std::stop_token stop) { Run(stop); });
    condition_.notify_one();
  } catch (...) {
    std::scoped_lock lock(mutex_);
    status_ = {AudioState::Error, E_OUTOFMEMORY};
  }
}

void AudioPlayer::Publish(std::uint64_t request, AudioSnapshot value) {
  std::scoped_lock lock(mutex_);
  // A slow open must never overwrite a newer Stop or Play request.
  if (request == request_)
    status_ = value;
}

void AudioPlayer::Run(std::stop_token stop) noexcept {
  using namespace winrt::Windows::Media::Playback;
  using namespace winrt::Windows::Media::Core;
  using namespace winrt::Windows::Storage::Streams;
  struct Events {
    std::atomic<std::int32_t> error{};
    std::atomic_bool ended{};
  };
  struct Apartment {
    bool initialized{};
    ~Apartment() {
      if (initialized)
        winrt::uninit_apartment();
    }
  } apartment;
  MediaPlayer player{nullptr};
  MediaSource source{nullptr};
  InMemoryRandomAccessStream stream{nullptr};
  MediaPlayer::MediaFailed_revoker failed;
  MediaPlayer::MediaEnded_revoker ended;
  std::shared_ptr<Events> events;
  auto close = [&]() noexcept {
    failed.revoke();
    ended.revoke();
    try {
      if (player)
        player.Close();
    } catch (...) {
    }
    player = nullptr;
    try {
      if (source)
        source.Close();
    } catch (...) {
    }
    source = nullptr;
    try {
      if (stream)
        stream.Close();
    } catch (...) {
    }
    stream = nullptr;
    events.reset();
  };
  std::uint64_t handled{};
  std::uint64_t handledVolume{};
  while (!stop.stop_requested()) {
    bool play{};
    std::uint64_t request{};
    double volume{};
    bool volumeChanged{};
    {
      std::unique_lock lock(mutex_);
      const auto ready = [&] {
        return stop.stop_requested() || request_ != handled ||
               volumeRequest_ != handledVolume;
      };
      if (player)
        condition_.wait_for(lock, std::chrono::milliseconds(50), ready);
      else
        condition_.wait(lock, ready);
      if (stop.stop_requested())
        break;
      request = request_;
      play = play_;
      volume = volume_;
      volumeChanged = handledVolume != volumeRequest_;
      handledVolume = volumeRequest_;
    }
    try {
      if (request != handled) {
        handled = request;
        close();
        if (!play) {
          Publish(handled, {AudioState::Stopped, 0});
          continue;
        }
        if (!apartment.initialized) {
          winrt::init_apartment(winrt::apartment_type::multi_threaded);
          apartment.initialized = true;
        }
        // Read through the process's filesystem hooks: a StorageFile
        // broker cannot see MO2's virtual Data tree. Only this worker
        // reads/decodes audio; Present only submits commands.
        std::ifstream file(track_, std::ios::binary | std::ios::ate);
        if (!file)
          winrt::throw_hresult(HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND));
        const auto size = file.tellg();
        if (size <= 0 || size > 16 * 1024 * 1024)
          winrt::throw_hresult(HRESULT_FROM_WIN32(ERROR_INVALID_DATA));
        std::vector<std::uint8_t> bytes(static_cast<std::size_t>(size));
        file.seekg(0);
        if (!file.read(reinterpret_cast<char *>(bytes.data()), size))
          winrt::throw_hresult(HRESULT_FROM_WIN32(ERROR_READ_FAULT));
        stream = InMemoryRandomAccessStream{};
        DataWriter writer(stream);
        writer.WriteBytes(bytes);
        writer.StoreAsync().get();
        writer.DetachStream();
        stream.Seek(0);
        source = MediaSource::CreateFromStream(stream, L"audio/mpeg");
        player = MediaPlayer{};
        player.CommandManager().IsEnabled(false);
        events = std::make_shared<Events>();
        failed = player.MediaFailed(
            winrt::auto_revoke,
            [signals = events](const auto &,
                               const MediaPlayerFailedEventArgs &args) {
              const auto error =
                  static_cast<std::int32_t>(args.ExtendedErrorCode());
              signals->error = error < 0 ? error : E_FAIL;
            });
        ended = player.MediaEnded(
            winrt::auto_revoke, [signals = events](const auto &, const auto &) {
              signals->ended = true;
            });
        player.AutoPlay(false);
        player.Volume(volume);
        player.Source(source);
        bool superseded{};
        {
          std::scoped_lock lock(mutex_);
          superseded = request_ != handled || stop.stop_requested();
        }
        if (superseded) {
          close();
          continue;
        }
        player.Play();
      }
      if (player && events) {
        // Gain changes stay on the media worker and never restart the track.
        if (volumeChanged)
          player.Volume(volume);
        if (const auto error = events->error.load(); error < 0) {
          close();
          Publish(handled, {AudioState::Error, error});
        } else if (events->ended.load()) {
          close();
          Publish(handled, {AudioState::Stopped, 0});
        } else if (player.PlaybackSession().PlaybackState() ==
                   MediaPlaybackState::Playing) {
          Publish(handled, {AudioState::Playing, 0});
        }
      }
    } catch (...) {
      const auto error = static_cast<std::int32_t>(winrt::to_hresult());
      close();
      Publish(handled, {AudioState::Error, error});
    }
  }
  close();
}
} // namespace TheosRenderPipeline
