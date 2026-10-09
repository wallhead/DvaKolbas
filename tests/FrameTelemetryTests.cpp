#include "FrameTelemetry.h"
#include <iostream>
#include <limits>

using namespace TheosRenderPipeline::Telemetry;

struct SwapChain
{
    std::uint32_t count{};
    long result{};
    unsigned queries{};
    long GetLastPresentCount(std::uint32_t* output)
    {
        ++queries;
        *output = count;
        return result;
    }
};

int main()
{
    unsigned failures{};
    const auto check = [&](bool value, const char* name) {
        if (!value) { std::cerr << name << '\n'; ++failures; }
    };
    SwapChain chain;
    check(!ReadDxgiOutputCounter<SwapChain>(nullptr, 1, 1, true).available, "null chain");
    check(!ReadDxgiOutputCounter(&chain, 1, 1, false).available && chain.queries == 0, "inactive query guard");
    chain.count = 500;
    auto counter = ReadDxgiOutputCounter(&chain, 7, 10, true);
    check(counter.available && counter.source == OutputSource::DXGI && counter.frames == 500 &&
        counter.epoch == 7 && counter.observations == 10, "native count preserved");
    for (const auto status : { -1L, 1L }) {
        chain.result = status;
        check(!ReadDxgiOutputCounter(&chain, 7, 11, true).available, "failed/status query unavailable");
    }
    chain.result = 0;
    OutputRateSampler sampler;
    for (unsigned step = 0; step <= 10; ++step) {
        chain.count = 500 + step * 6;
        sampler.Update(step * 100.0, ReadDxgiOutputCounter(&chain, 7, step, true));
    }
    check(sampler.Rate().available && std::abs(sampler.Rate().fps - 60.0f) < 0.01f, "actual 60 presents per second");
    chain.count = 1;
    sampler.Update(1100, ReadDxgiOutputCounter(&chain, 7, 11, true));
    check(!sampler.Rate().available, "rollback resets baseline");
    chain.count = std::numeric_limits<std::uint32_t>::max();
    sampler.Update(1200, ReadDxgiOutputCounter(&chain, 8, 12, true));
    check(!sampler.Rate().available, "new chain epoch resets baseline");
    chain.count = 0;
    sampler.Update(1300, ReadDxgiOutputCounter(&chain, 8, 13, true));
    check(!sampler.Rate().available, "32-bit wrap resets baseline");
    sampler.Update(1400, {});
    check(!sampler.Rate().available && sampler.Rate().source == OutputSource::Unavailable, "unavailable clears output");
    for (unsigned step = 0; step <= 10; ++step) {
        sampler.Update(1500 + step * 100.0, { OutputSource::Streamline, 9, step, step * 12, true });
    }
    check(sampler.Rate().available && std::abs(sampler.Rate().fps - 120.0f) < 0.01f, "streamline totals unchanged");
    sampler.Update(2800, { OutputSource::Streamline, 9, 11, 132, true });
    check(!sampler.Rate().available, "long interruption rebaselines");
    return failures ? 1 : 0;
}
