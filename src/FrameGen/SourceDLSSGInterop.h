#pragma once
#include "Graphics/D3D11D3D12Interop.h"
namespace TheosRenderPipeline::SourceDLSSG
{
    using Work = Graphics::InteropWork;
    using SharedTexture = Graphics::SharedTexture;
    using AllocatorWaitTiming = Graphics::AllocatorWaitTiming;
    using RetirementWaitPolicy = Graphics::RetirementWaitPolicy;
    using RetirementWaitDiagnostics = Graphics::RetirementWaitDiagnostics;
    inline constexpr auto kCommandSlots = Graphics::kCommandSlots;
    inline constexpr auto kPresentationSlots = Graphics::kPresentationSlots;
	struct InputWaitDiagnostics
	{
		const char* stage{ "not called" };
		// Address snapshots for failure logging only; these do not own COM objects.
		const void* hostIdentity{};
		const void* referenceFenceOwner{};
		const void* inputFenceOwner{};
	};

    // NVIDIA retains its completion-fence/presenting-queue reader bridge.
    class Interop final : public Graphics::D3D11D3D12Interop
    {
    public:
        HRESULT WaitForInputReaders(ID3D12Fence* fence, std::uint64_t value);
        const InputWaitDiagnostics& LastInputWait() const { return inputWait_; }
    private:
        InputWaitDiagnostics inputWait_;
    };
}
