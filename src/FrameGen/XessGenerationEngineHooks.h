#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <span>
#include <cstring>
#include <limits>
#include <optional>
#include "XessGenerationInputHandoff.h"

namespace TheosRenderPipeline::XessEngineHooks
{
    enum class Boundary { BeforeInput, InputSampled, BeforeRender };
    // The host descriptor and its context must remain allocated until process
    // exit. Unbinding prevents future dispatch, but is not a reclamation fence.
    struct Observer
    {
        void* context{};
        void (*observe)(void*, Boundary) noexcept{};
    };
    inline std::atomic<Observer*> observer{};
    struct HookWitness
    {
        std::uintptr_t site{},callee{};
        std::array<std::uint8_t,5> installedCall{};
    };
    // Written once before publishing installed; diagnostics read only.
    inline std::array<HookWitness,3> witnesses{};
    inline std::array<std::atomic<std::uint64_t>,3> boundaryCalls{};
    inline bool Bind(Observer* value)
    {
        if (!value || !value->observe) return false;
        Observer* empty{};
        return observer.compare_exchange_strong(empty,value,std::memory_order_release,std::memory_order_relaxed);
    }
    inline bool Unbind(Observer* value)
    { return value && observer.compare_exchange_strong(value,nullptr,std::memory_order_acq_rel,std::memory_order_relaxed); }
    inline void Observe(Boundary boundary) noexcept
    {
        boundaryCalls[static_cast<unsigned>(boundary)].fetch_add(1,std::memory_order_relaxed);
        if (auto* value=observer.load(std::memory_order_acquire)) value->observe(value->context,boundary);
    }
    template<class Poll>
    inline void PollMainInput(Poll&& poll)
    {
        // Start at the observed main input call, before its preserved callee.
        // An outer update caller can be bypassed by another mod's detour.
        Observe(Boundary::BeforeInput);
        poll();
        Observe(Boundary::InputSampled);
    }

    // Captured from the identified running 1.6.1170 executable. Includes the
    // argument setup and the entire direct call, not just its E8 opcode.
    inline constexpr std::array<std::uint8_t,20> inputBytes{
        0xF3,0x0F,0x10,0x0D,0x8C,0x5E,0xB8,0x02,0x48,0x8B,0x0D,0x99,0x6A,0xB1,0x02,0xE8,0x34,0x2B,0x69,0x00};
    inline constexpr std::array<std::uint8_t,14> renderBytes{
        0x33,0xD2,0x48,0x8D,0x0D,0x79,0x4B,0xC4,0x02,0xE8,0x44,0x09,0x80,0x00};
    // Native input job 36578, including its argument setup, stack restoration
    // and tail jump into the same preserved PollInputDevices entry. This is
    // instruction evidence only; the job's thread/order still needs a trace.
    inline constexpr std::array<std::uint8_t,40> gameplayInputBytes{
        0x48,0x83,0xEC,0x28,0x48,0x8B,0x0D,0x2D,0x23,0xB2,0x02,0xE8,0xE0,0x6A,0x40,0x00,
        0xF3,0x0F,0x10,0x0D,0x64,0x4E,0xB8,0x02,0x48,0x8B,0x0D,0x71,0x5A,0xB1,0x02,
        0x48,0x83,0xC4,0x28,0xE9,0x08,0x1B,0x69,0x00};
    inline bool Qualified(std::array<std::uint16_t,4> version,
        std::span<const std::uint8_t> input,std::span<const std::uint8_t> render,
        std::span<const std::uint8_t> gameplayInput={})
    {
        const auto matches=[](auto actual,const auto& expected) {
            if (actual.size()!=expected.size()) return false;
            for (std::size_t i=0;i<actual.size();++i) if (actual[i]!=expected[i]) return false;
            return true;
        };
        return version==std::array<std::uint16_t,4>{1,6,1170,0} &&
            matches(input,inputBytes) && matches(render,renderBytes) && matches(gameplayInput,gameplayInputBytes);
    }
    // Diagnostic records are atomic because native jobs may use worker threads.
    // They never create timing markers, source IDs or interpolation admission.
    struct GameplayInputProbe
    {
        std::atomic<std::uint64_t> started{},completed{},beginTicks{},endTicks{};
        std::atomic<std::uint32_t> beginThread{},endThread{};
        template<class Poll,class Stamp>
        void PollInput(Poll&& poll,Stamp&& stamp)
        {
            const auto begin=stamp();
            beginThread.store(begin.first,std::memory_order_relaxed);
            beginTicks.store(begin.second,std::memory_order_relaxed);
            started.fetch_add(1,std::memory_order_release);
            poll();
            const auto end=stamp();
            endThread.store(end.first,std::memory_order_relaxed);
            endTicks.store(end.second,std::memory_order_relaxed);
            completed.fetch_add(1,std::memory_order_release);
        }
    };
    inline GameplayInputProbe gameplayProbe;
    inline XessGenerationInputHandoff gameplayInput;
    // The inspected job's first eleven bytes contain a RIP-relative singleton
    // load. Relocate that complete instruction before jumping to its original
    // first call. Generic prologue copying would use the wrong singleton.
    inline std::optional<std::array<std::uint8_t,25>> RelocateGameplayEntry(
        std::uintptr_t site,std::uintptr_t trampoline,std::span<const std::uint8_t> actual)
    {
        if(!site || !trampoline || actual.size()!=gameplayInputBytes.size())return {};
        for(std::size_t i=0;i<actual.size();++i)if(actual[i]!=gameplayInputBytes[i])return {};
        std::int32_t original{};std::memcpy(&original,actual.data()+7,4);
        const auto displacement=static_cast<std::int64_t>(site)-static_cast<std::int64_t>(trampoline)+original;
        if(displacement<std::numeric_limits<std::int32_t>::min() || displacement>std::numeric_limits<std::int32_t>::max())return {};
        std::array<std::uint8_t,25> code{};
        std::memcpy(code.data(),actual.data(),11);
        const auto relocated=static_cast<std::int32_t>(displacement);std::memcpy(code.data()+7,&relocated,4);
        code[11]=0xFF;code[12]=0x25; // jmp qword ptr [rip+0], no scratch register
        const std::uint64_t resume=site+11;std::memcpy(code.data()+17,&resume,8);
        return code;
    }
    inline std::atomic_bool installed{};
}
