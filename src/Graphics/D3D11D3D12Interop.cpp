#include "D3D11D3D12Interop.h"

#include <dxgi1_4.h>
#include <chrono>
#include <limits>
#include <utility>

namespace TheosRenderPipeline::Graphics
{
	D3D11D3D12Interop::~D3D11D3D12Interop()
	{
		if (FAILED(Drain())) {
			// A timeout is not retirement. Keep GPU-owned objects alive until the
			// process ends rather than freeing an allocator that is still in use.
			AbandonInFlightObjects();
		}
		for (auto& work : work_) {
			if (work.event) {
				::CloseHandle(work.event);
			}
		}
	}

	HRESULT D3D11D3D12Interop::Check(HRESULT a_result)
	{
		if (FAILED(a_result) && SUCCEEDED(fault_)) {
			fault_ = a_result;
		}
		return a_result;
	}

	D3D11D3D12Interop::WorkContext* D3D11D3D12Interop::Get(InteropWork a_work)
	{
		const auto index = static_cast<std::size_t>(a_work);
		return index < work_.size() ? &work_[index] : nullptr;
	}

	HRESULT D3D11D3D12Interop::Initialize(ID3D11Device* a_device11, ID3D12Device* a_device12,
		ID3D12CommandQueue* a_queue)
	{
		if (device11_ || !a_device11 || !a_device12 || !a_queue) {
			return E_INVALIDARG;
		}
		Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
		Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
		DXGI_ADAPTER_DESC adapterDesc{};
		HRESULT hr = a_device11->QueryInterface(IID_PPV_ARGS(&dxgiDevice));
		if (FAILED(hr)) { return Check(hr); }
		if (FAILED(hr = dxgiDevice->GetAdapter(&adapter))) { return Check(hr); }
		if (FAILED(hr = adapter->GetDesc(&adapterDesc))) { return Check(hr); }
		const auto luid = a_device12->GetAdapterLuid();
		if (luid.HighPart != adapterDesc.AdapterLuid.HighPart ||
			luid.LowPart != adapterDesc.AdapterLuid.LowPart ||
			a_queue->GetDesc().Type != D3D12_COMMAND_LIST_TYPE_DIRECT) {
			return E_INVALIDARG;
		}
		Microsoft::WRL::ComPtr<ID3D12Device> queueDevice;
		Microsoft::WRL::ComPtr<IUnknown> suppliedIdentity, queueIdentity;
		if (FAILED(hr = a_queue->GetDevice(IID_PPV_ARGS(&queueDevice))) ||
			FAILED(hr = a_device12->QueryInterface(IID_PPV_ARGS(&suppliedIdentity))) ||
			FAILED(hr = queueDevice.As(&queueIdentity))) { return Check(hr); }
		if (suppliedIdentity.Get() != queueIdentity.Get()) { return E_INVALIDARG; }
		if (FAILED(hr = a_device11->QueryInterface(IID_PPV_ARGS(&device11_)))) { return Check(hr); }
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate;
		a_device11->GetImmediateContext(&immediate);
		if (FAILED(hr = immediate.As(&context11_))) { return Check(hr); }
		device12_ = a_device12;
		queue_ = a_queue;
		for (auto& work : work_) {
			if (FAILED(hr = device12_->CreateFence(0, D3D12_FENCE_FLAG_SHARED,
				IID_PPV_ARGS(&work.fence12)))) { return Check(hr); }
			HANDLE handle = nullptr;
			hr = device12_->CreateSharedHandle(work.fence12.Get(), nullptr, GENERIC_ALL, nullptr, &handle);
			if (FAILED(hr)) { return Check(hr); }
			hr = device11_->OpenSharedFence(handle, IID_PPV_ARGS(&work.fence11));
			::CloseHandle(handle);
			if (FAILED(hr)) { return Check(hr); }
			work.event = ::CreateEventW(nullptr, FALSE, FALSE, nullptr);
			if (!work.event) { return Check(HRESULT_FROM_WIN32(::GetLastError())); }
			for (std::size_t slot = 0; slot < kCommandSlots; ++slot) {
				if (FAILED(hr = device12_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
					IID_PPV_ARGS(&work.allocators[slot])))) { return Check(hr); }
				if (FAILED(hr = device12_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
					work.allocators[slot].Get(), nullptr, IID_PPV_ARGS(&work.lists[slot])))) { return Check(hr); }
				if (FAILED(hr = work.lists[slot]->Close())) { return Check(hr); }
			}
		}
		// ReShade exposes a proxy device/queue but forwards fence creation to the
		// underlying device. Anchor that second identity to a fence we created,
		// not to an adapter LUID or an identity supplied by the incoming fence.
		Microsoft::WRL::ComPtr<ID3D12Device> fenceDevice;
		if (FAILED(hr = Get(InteropWork::FrameGeneration)->fence12->GetDevice(IID_PPV_ARGS(&fenceDevice))) ||
			FAILED(hr = fenceDevice.As(&fenceDeviceIdentity_))) { return Check(hr); }
		ready_ = true;
		return S_OK;
	}

	HRESULT D3D11D3D12Interop::CreateSharedTexture(const D3D11_TEXTURE2D_DESC& a_desc, SharedTexture& a_output)
	{
		if (!Ready()) { return FAILED(fault_) ? fault_ : E_UNEXPECTED; }
		if (a_output.texture11 || a_output.texture12 ||
			a_desc.Usage != D3D11_USAGE_DEFAULT || a_desc.CPUAccessFlags ||
			(a_desc.BindFlags & D3D11_BIND_DEPTH_STENCIL) ||
			!a_desc.Width || !a_desc.Height || a_desc.MipLevels != 1 || a_desc.ArraySize != 1 ||
			a_desc.SampleDesc.Count != 1 || a_desc.SampleDesc.Quality != 0 ||
			a_desc.Format == DXGI_FORMAT_UNKNOWN) { return E_INVALIDARG; }
		D3D12_RESOURCE_DESC desc{};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Width = a_desc.Width;
		desc.Height = a_desc.Height;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = a_desc.Format;
		desc.SampleDesc.Count = 1;
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS;
		if (a_desc.BindFlags & D3D11_BIND_RENDER_TARGET) { desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET; }
		if (a_desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS) { desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS; }
		D3D12_HEAP_PROPERTIES heap{};
		heap.Type = D3D12_HEAP_TYPE_DEFAULT;
		SharedTexture result;
		auto hr = device12_->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_SHARED,
			&desc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&result.texture12));
		if (FAILED(hr)) { return Check(hr); }
		HANDLE handle = nullptr;
		hr = device12_->CreateSharedHandle(result.texture12.Get(), nullptr, GENERIC_ALL, nullptr, &handle);
		if (FAILED(hr)) { return Check(hr); }
		hr = device11_->OpenSharedResource1(handle, IID_PPV_ARGS(&result.texture11));
		::CloseHandle(handle);
		if (FAILED(hr)) { return Check(hr); }
		result.texture11->GetDesc(&result.desc);
		a_output = std::move(result);
		return S_OK;
	}

	HRESULT D3D11D3D12Interop::CopyInput(ID3D11Texture2D* a_input, const SharedTexture& a_destination)
	{
		if (!Ready()) { return FAILED(fault_) ? fault_ : E_UNEXPECTED; }
        if (srActive_ && (srProducerSubmitted_ || Get(InteropWork::Upscaling)->recording ||
            (srDispatchSubmitted_ && !srConsumerQueued_))) { return E_UNEXPECTED; }
		if (!a_input || !a_destination.texture11 || a_input == a_destination.texture11.Get()) { return E_INVALIDARG; }
		Microsoft::WRL::ComPtr<ID3D11Device> sourceDevice, destinationDevice;
		a_input->GetDevice(&sourceDevice);
		a_destination.texture11->GetDevice(&destinationDevice);
		if (sourceDevice.Get() != static_cast<ID3D11Device*>(device11_.Get()) ||
			destinationDevice.Get() != sourceDevice.Get()) { return E_INVALIDARG; }
		D3D11_TEXTURE2D_DESC desc{};
		a_input->GetDesc(&desc);
		if (desc.Width != a_destination.desc.Width || desc.Height != a_destination.desc.Height ||
			desc.Format != a_destination.desc.Format || desc.ArraySize != 1 || desc.MipLevels != 1 ||
			desc.SampleDesc.Count != 1 || desc.SampleDesc.Quality != 0) { return E_INVALIDARG; }
		context11_->CopyResource(a_destination.texture11.Get(), a_input);
		return S_OK;
	}

	HRESULT D3D11D3D12Interop::CopyInputRegion(ID3D11Texture2D* a_input, const SharedTexture& a_destination, FrameExtent a_extent)
	{
		if (!Ready()) { return FAILED(fault_) ? fault_ : E_UNEXPECTED; }
		if (a_destination.desc.Width != a_extent.width || a_destination.desc.Height != a_extent.height) { return E_INVALIDARG; }
		return D3D11FrameCopy::Color(context11_.Get(), a_input, a_destination.texture11.Get(), a_extent);
	}

	HRESULT D3D11D3D12Interop::SignalD3D11(InteropWork a_work)
	{
		auto* work = Get(a_work);
		if (!Ready() || !work || work->recording) { return E_UNEXPECTED; }
		if (work->value) {
			const auto wait = context11_->Wait(work->fence11.Get(), work->value);
			if (FAILED(wait)) { return Check(wait); }
		}
		const auto hr = context11_->Signal(work->fence11.Get(), ++work->value);
		context11_->Flush(); if(performanceSink_.flush)performanceSink_.flush(performanceSink_.owner);
		return Check(hr);
	}

	HRESULT D3D11D3D12Interop::WaitD3D12(InteropWork a_work)
	{
		auto* work = Get(a_work);
		if (!Ready() || !work) { return E_UNEXPECTED; }
		return work->value ? Check(queue_->Wait(work->fence12.Get(), work->value)) : S_OK;
	}

	HRESULT D3D11D3D12Interop::WaitD3D11(InteropWork a_work)
	{
		auto* work = Get(a_work);
		if (!Ready() || !work) { return E_UNEXPECTED; }
		return work->value ? Check(context11_->Wait(work->fence11.Get(), work->value)) : S_OK;
	}

	HRESULT D3D11D3D12Interop::WaitCPU(WorkContext& a_work, std::uint64_t a_value, AllocatorWaitTiming* a_timing)
	{
		constexpr auto removed = (std::numeric_limits<std::uint64_t>::max)();
		if (!a_value) { if(performanceSink_.wait)performanceSink_.wait(performanceSink_.owner,false,0);return S_OK; }
		const auto completed = a_work.fence12->GetCompletedValue();
		if (completed == removed) {
			return Check(DXGI_ERROR_DEVICE_REMOVED);
		}
		if (completed >= a_value) { if(performanceSink_.wait)performanceSink_.wait(performanceSink_.owner,false,0);return S_OK; }
		const auto begin = std::chrono::steady_clock::now();
		auto hr = a_work.fence12->SetEventOnCompletion(a_value, a_work.event);
		if (FAILED(hr)) { return Check(hr); }
		// A slow GPU after a cell load can take seconds per frame; that is not a
		// fault. Keep waiting while the shared fence (D3D11 producer and D3D12
		// queue) advances. Nothing is reset until the target actually retires.
		RetirementWaitDiagnostics wait{ static_cast<InteropWork>(&a_work - work_.data()), a_value, completed, completed };
		auto progress = completed;
		DWORD stalledMs = 0;
        std::uint64_t blockedNanoseconds{};
		for (;;) {
            const auto blockBegin=performanceSink_.wait?std::chrono::steady_clock::now():std::chrono::steady_clock::time_point{};
			const auto result = ::WaitForSingleObject(a_work.event, waitPolicy_.sliceMs);
            if(performanceSink_.wait)blockedNanoseconds+=static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now()-blockBegin).count());
			wait.completedAtEnd = a_work.fence12->GetCompletedValue();
			if (result == WAIT_OBJECT_0) {
				if (wait.completedAtEnd >= a_value) { break; }
				// An earlier wait can return after its target retired but before
				// that registration set the auto-reset event. This one stays armed.
				continue;
			}
			++wait.slices;
			if (result != WAIT_TIMEOUT) { hr = HRESULT_FROM_WIN32(::GetLastError()); break; }
			if (wait.completedAtEnd == removed) { hr = DXGI_ERROR_DEVICE_REMOVED; break; }
			if (FAILED(hr = device12_->GetDeviceRemovedReason())) { break; }
			if (wait.completedAtEnd >= a_value) { break; }
			if (wait.completedAtEnd > progress) {
				progress = wait.completedAtEnd;
				stalledMs = 0;
			} else if ((stalledMs += waitPolicy_.sliceMs) >= waitPolicy_.stallLimitMs) {
				hr = HRESULT_FROM_WIN32(WAIT_TIMEOUT);
				break;
			}
		}
		const auto elapsed = std::chrono::steady_clock::now() - begin;
        if(performanceSink_.wait)performanceSink_.wait(performanceSink_.owner,true,blockedNanoseconds);
		if (a_timing) {
			a_timing->waited = true;
			a_timing->nanoseconds = static_cast<std::uint64_t>(
				std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count());
		}
		if (SUCCEEDED(hr)) {
			if (wait.completedAtEnd == removed) { hr = DXGI_ERROR_DEVICE_REMOVED; }
			else if (wait.completedAtEnd < a_value) { hr = E_FAIL; }
		}
		if (wait.slices) {
			wait.elapsedMs = static_cast<std::uint64_t>(
				std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count());
			wait.result = hr;
			extendedWait_ = wait;
			extendedWaitPending_ = true;
		}
		return FAILED(hr) ? Check(hr) : S_OK;
	}

	bool D3D11D3D12Interop::TakeExtendedWait(RetirementWaitDiagnostics& a_wait)
	{
		if (!extendedWaitPending_) { return false; }
		a_wait = extendedWait_;
		extendedWaitPending_ = false;
		return true;
	}

	HRESULT D3D11D3D12Interop::Begin(InteropWork a_work, ID3D12GraphicsCommandList** a_list, AllocatorWaitTiming* a_wait)
	{
		if (a_wait) { *a_wait = {}; }
		if (!a_list) { return E_POINTER; }
		*a_list = nullptr;
		auto* work = Get(a_work);
		if (!Ready() || !work || work->recording) { return E_UNEXPECTED; }
		auto hr = WaitD3D12(a_work);
		if (FAILED(hr)) { return hr; }
		if (FAILED(hr = WaitCPU(*work, work->submitted[work->slot], a_wait))) { return hr; }
		if (FAILED(hr = work->allocators[work->slot]->Reset())) { return Check(hr); }
		if (FAILED(hr = work->lists[work->slot]->Reset(work->allocators[work->slot].Get(), nullptr))) { return Check(hr); }
		work->recording = true;
		*a_list = work->lists[work->slot].Get();
		return S_OK;
	}

	HRESULT D3D11D3D12Interop::Submit(InteropWork a_work)
	{
		auto* work = Get(a_work);
		if (!Ready() || !work || !work->recording) { return E_UNEXPECTED; }
		auto hr = work->lists[work->slot]->Close();
		work->recording = false;
		if (FAILED(hr)) { return Check(hr); }
		ID3D12CommandList* lists[]{ work->lists[work->slot].Get() };
		queue_->ExecuteCommandLists(1, lists);
		hr = queue_->Signal(work->fence12.Get(), ++work->value);
		if (FAILED(hr)) { return Check(hr); }
		work->submitted[work->slot] = work->value;
		work->slot = (work->slot + 1) % kCommandSlots;
		return S_OK;
	}

	HRESULT D3D11D3D12Interop::Drain()
	{
        // Recording can still contain context/descriptor references which a
        // caller may submit later. It is not a completed lifetime boundary.
        for (const auto& work : work_) if (work.recording) { return E_UNEXPECTED; }
        // The last D3D12 dispatch fence does not retire the D3D11 output
        // readers queued after WaitConsumer. Signal behind those readers and
        // wait for that real consumer submission before releasing resources.
        if (srActive_ && srConsumerQueued_) {
            auto* work = Get(InteropWork::Upscaling);
            const auto hr = context11_->Signal(work->fence11.Get(), ++work->value);
            context11_->Flush(); if(performanceSink_.flush)performanceSink_.flush(performanceSink_.owner);
            if (FAILED(hr)) { return Check(hr); }
            srConsumerQueued_ = false;
        }
		if (context11_) { context11_->Flush(); if(performanceSink_.flush)performanceSink_.flush(performanceSink_.owner); }
		for (auto& work : work_) {
			if (work.fence12 && work.value) {
				const auto hr = WaitCPU(work, work.value);
				if (FAILED(hr)) { return hr; }
			}
		}
        srProducerSubmitted_ = false;
        srDispatchSubmitted_ = false;
		return S_OK;
	}

    HRESULT D3D11D3D12Interop::SignalProducer()
    {
        if (!Ready() || srProducerSubmitted_ || Get(InteropWork::Upscaling)->recording ||
            (srDispatchSubmitted_ && !srConsumerQueued_)) { return E_UNEXPECTED; }
        if (const auto removed = device12_->GetDeviceRemovedReason(); FAILED(removed)) { return Check(removed); }
        const auto hr = SignalD3D11(InteropWork::Upscaling);
        if (SUCCEEDED(hr)) {
            // This submitted D3D11 signal follows prior output readers and
            // current input copies. Begin queues a D3D12 wait on this value.
            srActive_ = true; srProducerSubmitted_ = true;
            srDispatchSubmitted_ = false; srConsumerQueued_ = false;
        }
        return hr;
    }
    HRESULT D3D11D3D12Interop::Begin(ID3D12GraphicsCommandList** list)
    {
        if (!list) { return E_POINTER; }
        *list = nullptr;
        if (!srProducerSubmitted_ || srDispatchSubmitted_) { return E_UNEXPECTED; }
        return Begin(InteropWork::Upscaling, list);
    }
    HRESULT D3D11D3D12Interop::Submit()
    {
        if (!srProducerSubmitted_) { return E_UNEXPECTED; }
        const auto hr = Submit(InteropWork::Upscaling);
        if (SUCCEEDED(hr)) { srProducerSubmitted_ = false; srDispatchSubmitted_ = true; }
        return hr;
    }
    HRESULT D3D11D3D12Interop::WaitConsumer()
    {
        if (!srDispatchSubmitted_ || srConsumerQueued_) { return E_UNEXPECTED; }
        const auto hr = WaitD3D11(InteropWork::Upscaling);
        if (SUCCEEDED(hr)) { srConsumerQueued_ = true; }
        return hr;
    }

    HRESULT D3D11D3D12Interop::DiscardRecording()
    {
        auto* work=Get(InteropWork::Upscaling);
        if(!Ready() || !srProducerSubmitted_ || srDispatchSubmitted_ || !work || !work->recording)return E_UNEXPECTED;
        const auto hr=work->lists[work->slot]->Close();
        if(FAILED(hr))return Check(hr);
        work->recording=false;
        // The next Begin resets this closed list after the slot's previous real
        // submission retires. Current producer work still needs a normal Drain.
        return S_OK;
    }

	std::uint64_t D3D11D3D12Interop::LastValue(InteropWork a_work) const
	{
		const auto index = static_cast<std::size_t>(a_work);
		return index < work_.size() ? work_[index].value : 0;
	}

    HRESULT D3D11D3D12Interop::DiscardUnsubmitted(InteropWork kind)
    {
        if(kind==InteropWork::Upscaling)return DiscardRecording();
        auto* work=Get(kind);if(!Ready() || !work || !work->recording)return E_UNEXPECTED;
        const auto hr=work->lists[work->slot]->Close();if(FAILED(hr))return Check(hr);
        work->recording=false;return S_OK;
    }

	std::size_t D3D11D3D12Interop::CurrentSlot(InteropWork a_work) const
	{
		const auto index = static_cast<std::size_t>(a_work);
		return index < work_.size() ? work_[index].slot : kCommandSlots;
	}

	HRESULT D3D11D3D12Interop::RecordCopy(ID3D12GraphicsCommandList* a_list,
		ID3D12Resource* a_source, ID3D12Resource* a_destination)
	{
		if (!a_list || !a_source || !a_destination || a_source == a_destination) { return E_INVALIDARG; }
		const auto source = a_source->GetDesc();
		const auto destination = a_destination->GetDesc();
		if (source.Dimension != destination.Dimension || source.Width != destination.Width ||
			source.Height != destination.Height || source.Format != destination.Format ||
			source.DepthOrArraySize != destination.DepthOrArraySize || source.MipLevels != destination.MipLevels ||
			source.SampleDesc.Count != destination.SampleDesc.Count || source.SampleDesc.Quality != destination.SampleDesc.Quality) {
			return E_INVALIDARG;
		}
		D3D12_RESOURCE_BARRIER barriers[2]{};
		for (auto& barrier : barriers) {
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		}
		barriers[0].Transition.pResource = a_source;
		barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
		barriers[1].Transition.pResource = a_destination;
		barriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
		a_list->ResourceBarrier(2, barriers);
		a_list->CopyResource(a_destination, a_source);
		for (auto& barrier : barriers) { std::swap(barrier.Transition.StateBefore, barrier.Transition.StateAfter); }
		a_list->ResourceBarrier(2, barriers);
		return S_OK;
	}

	void D3D11D3D12Interop::AbandonInFlightObjects()
	{
		for (auto& work : work_) {
			for (auto& list : work.lists) { (void)list.Detach(); }
			for (auto& allocator : work.allocators) { (void)allocator.Detach(); }
			(void)work.fence11.Detach();
			(void)work.fence12.Detach();
			// SetEventOnCompletion may still reference this event after timeout.
			work.event = nullptr;
		}
		(void)queue_.Detach();
		(void)context11_.Detach();
		(void)device11_.Detach();
		(void)device12_.Detach();
		(void)fenceDeviceIdentity_.Detach();
	}
}
