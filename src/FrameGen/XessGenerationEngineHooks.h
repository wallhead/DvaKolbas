#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <span>

namespace TheosRenderPipeline::XessEngineHooks
{
    enum class Boundary { BeforeUpdate, InputSampled, BeforeRender, AfterUpdate };
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
    inline std::array<std::atomic<std::uint64_t>,4> boundaryCalls{};
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

    // Captured from the identified running 1.6.1170 executable. Includes the
    // argument setup and the entire direct call, not just its E8 opcode.
    inline constexpr std::array<std::uint8_t,12> updateBytes{
        0x48,0x8B,0x0D,0xD1,0x56,0xB4,0x02,0xE8,0xAC,0x42,0x00,0x00};
    inline constexpr std::array<std::uint8_t,20> inputBytes{
        0xF3,0x0F,0x10,0x0D,0x8C,0x5E,0xB8,0x02,0x48,0x8B,0x0D,0x99,0x6A,0xB1,0x02,0xE8,0x34,0x2B,0x69,0x00};
    inline constexpr std::array<std::uint8_t,14> renderBytes{
        0x33,0xD2,0x48,0x8D,0x0D,0x79,0x4B,0xC4,0x02,0xE8,0x44,0x09,0x80,0x00};
    inline bool Qualified(std::array<std::uint16_t,4> version,std::span<const std::uint8_t> update,
        std::span<const std::uint8_t> input,std::span<const std::uint8_t> render)
    {
        const auto matches=[](auto actual,const auto& expected) {
            if (actual.size()!=expected.size()) return false;
            for (std::size_t i=0;i<actual.size();++i) if (actual[i]!=expected[i]) return false;
            return true;
        };
        return version==std::array<std::uint16_t,4>{1,6,1170,0} &&
            matches(update,updateBytes) && matches(input,inputBytes) && matches(render,renderBytes);
    }
    inline std::atomic_bool installed{};
}
