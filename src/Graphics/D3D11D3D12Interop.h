#pragma once

#include <d3d11_4.h>
#include <d3d12.h>
#include <wrl/client.h>
#include "FrameGen/D3D11FrameCopy.h"

#include <array>
#include <cstddef>
#include <cstdint>

namespace TheosRenderPipeline::Graphics
{
	// Three allocator slots per work type, independent of the two presentation
	// buffers. A slot may only reset after its recorded GPU submission retires.
	inline constexpr std::size_t kCommandSlots = 3;
	inline constexpr std::size_t kPresentationSlots = 2;
	enum class InteropWork : std::size_t { Upscaling, FrameGeneration, SwapChain, Count };

	struct AllocatorWaitTiming
	{
		std::uint64_t nanoseconds{};
		bool waited{};
	};
    // Optional observer only; ordinary FSR/NR-off targets need no NR library.
    struct InteropPerformanceSink {
        void* owner{};
        void(*wait)(void*,bool,std::uint64_t){};
        void(*flush)(void*){};
        void(*submitted)(void*,InteropWork,ID3D12Fence*,std::uint64_t){};
    };

	struct SharedTexture
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture11;
		Microsoft::WRL::ComPtr<ID3D12Resource> texture12;
		D3D11_TEXTURE2D_DESC desc{};
	};

	// A CPU retirement wait continues while its fence advances. Only a stall of
	// stallLimitMs without progress, or device removal, is a failure.
	struct RetirementWaitPolicy
	{
		DWORD sliceMs{ 2000 };
		DWORD stallLimitMs{ 20000 };
	};

	// Recorded when a retirement wait outlasts one slice, for failure logging.
	struct RetirementWaitDiagnostics
	{
		InteropWork work{ InteropWork::Count };
		std::uint64_t target{};
		std::uint64_t completedAtStart{};
		std::uint64_t completedAtEnd{};
		std::uint64_t elapsedMs{};
		std::uint32_t slices{};
		HRESULT result{ S_OK };
	};

	class D3D11D3D12Interop
	{
	public:
		// All methods run on the host render thread. Owners must keep shared
		// textures alive through a successful Drain before replacing/releasing.
		D3D11D3D12Interop() = default;
		virtual ~D3D11D3D12Interop();
		D3D11D3D12Interop(const D3D11D3D12Interop&) = delete;
		D3D11D3D12Interop& operator=(const D3D11D3D12Interop&) = delete;

		// Both devices and the backend DIRECT queue refer to the same adapter/device.
		HRESULT Initialize(ID3D11Device* a_device11, ID3D12Device* a_device12,
			ID3D12CommandQueue* a_queue);
		HRESULT CreateSharedTexture(const D3D11_TEXTURE2D_DESC& a_desc, SharedTexture& a_output);
		HRESULT CopyInput(ID3D11Texture2D* a_input, const SharedTexture& a_destination);
		HRESULT CopyInputRegion(ID3D11Texture2D* a_input, const SharedTexture& a_destination, FrameExtent a_extent);
		HRESULT SignalD3D11(InteropWork a_work);
		HRESULT WaitD3D12(InteropWork a_work);
		HRESULT WaitD3D11(InteropWork a_work);
		HRESULT Begin(InteropWork a_work, ID3D12GraphicsCommandList** a_list, AllocatorWaitTiming* a_wait = nullptr);
		HRESULT Submit(InteropWork a_work);
		// Ordinary SR adds strict producer order and final D3D11 reader retirement.
        HRESULT SignalProducer();
        // Snapshot only a genuinely submitted ordinary D3D11 producer signal.
        HRESULT ProducerDependency(ID3D12Fence** a_fence,std::uint64_t* a_value);
        HRESULT Begin(ID3D12GraphicsCommandList** a_list);
        HRESULT Submit();
        // Discard an ordinary list which has never reached ExecuteCommandLists.
        // No fence completion or GPU resource-state transition is fabricated.
        HRESULT DiscardRecording();
        HRESULT DiscardUnsubmitted(InteropWork);
        HRESULT WaitConsumer();
        HRESULT Drain();
        ID3D12Device* Device12() const { return device12_.Get(); }
        ID3D12CommandQueue* Queue() const { return queue_.Get(); }
        ID3D11DeviceContext4* Context11() const { return context11_.Get(); }

		void SetRetirementWaitPolicy(RetirementWaitPolicy a_policy) { waitPolicy_ = a_policy; }
        void SetPerformanceSink(InteropPerformanceSink sink){performanceSink_=sink;}
		// Returns and clears the most recent wait that outlasted one slice.
		bool TakeExtendedWait(RetirementWaitDiagnostics& a_wait);
		HRESULT Fault() const { return fault_; }
		bool Ready() const { return ready_ && SUCCEEDED(fault_); }
		std::uint64_t LastValue(InteropWork a_work) const;
		std::size_t CurrentSlot(InteropWork a_work) const;

		// Both resources enter and leave COMMON.
		static HRESULT RecordCopy(ID3D12GraphicsCommandList* a_list,
			ID3D12Resource* a_source, ID3D12Resource* a_destination);

	protected:
		struct WorkContext
		{
			Microsoft::WRL::ComPtr<ID3D12Fence> fence12;
			Microsoft::WRL::ComPtr<ID3D11Fence> fence11;
			std::array<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>, kCommandSlots> allocators;
			std::array<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>, kCommandSlots> lists;
			std::array<std::uint64_t, kCommandSlots> submitted{};
			std::uint64_t value{ 0 };
			std::size_t slot{ 0 };
			HANDLE event{ nullptr };
			bool recording{ false };
		};
		WorkContext* Get(InteropWork a_work);
		HRESULT Check(HRESULT a_result);
		HRESULT WaitCPU(WorkContext& a_work, std::uint64_t a_value, AllocatorWaitTiming* a_timing = nullptr);
		void AbandonInFlightObjects();

		Microsoft::WRL::ComPtr<ID3D11Device5> device11_;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext4> context11_;
		Microsoft::WRL::ComPtr<ID3D12Device> device12_;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> queue_;
		Microsoft::WRL::ComPtr<IUnknown> fenceDeviceIdentity_;
		std::array<WorkContext, static_cast<std::size_t>(InteropWork::Count)> work_;
		RetirementWaitPolicy waitPolicy_;
        InteropPerformanceSink performanceSink_;
		RetirementWaitDiagnostics extendedWait_;
		bool extendedWaitPending_{ false };
		HRESULT fault_{ S_OK };
		bool ready_{ false };
        bool srActive_{}, srProducerSubmitted_{}, srDispatchSubmitted_{}, srConsumerQueued_{};
	};
}
