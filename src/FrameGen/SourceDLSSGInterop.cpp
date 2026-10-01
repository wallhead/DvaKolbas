#include "SourceDLSSGInterop.h"
namespace TheosRenderPipeline::SourceDLSSG
{
	HRESULT Interop::WaitForInputReaders(ID3D12Fence* a_fence, std::uint64_t a_value)
	{
		inputWait_ = {};
		inputWait_.stage = "preconditions";
		auto* work = Get(Work::FrameGeneration);
		if (!Ready() || work->recording || (!a_fence && a_value)) { return E_UNEXPECTED; }
		if (a_fence) {
			Microsoft::WRL::ComPtr<ID3D12Device> owner;
			Microsoft::WRL::ComPtr<IUnknown> ownerIdentity, deviceIdentity;
			inputWait_.stage = "fence GetDevice";
			auto hr = a_fence->GetDevice(IID_PPV_ARGS(&owner));
			if (FAILED(hr)) { return Check(hr); }
			inputWait_.stage = "fence owner IUnknown";
			if (FAILED(hr = owner.As(&ownerIdentity))) { return Check(hr); }
			inputWait_.stage = "host IUnknown";
			if (FAILED(hr = device12_.As(&deviceIdentity))) { return Check(hr); }
			inputWait_.hostIdentity = deviceIdentity.Get();
			inputWait_.referenceFenceOwner = fenceDeviceIdentity_.Get();
			inputWait_.inputFenceOwner = ownerIdentity.Get();
			inputWait_.stage = "device identity";
			if (ownerIdentity != deviceIdentity && ownerIdentity != fenceDeviceIdentity_) { return E_INVALIDARG; }
		}
		inputWait_.stage = "prior frame queue Wait";
		auto hr = WaitD3D12(Work::FrameGeneration);
		if (FAILED(hr)) { return hr; }
		inputWait_.stage = "input fence queue Wait";
		if (a_fence && a_value && FAILED(hr = queue_->Wait(a_fence, a_value))) { return Check(hr); }
		// This queue signal also follows the most recent native Present. It
		// covers the default DLSS-G presenting-queue block when no fence is given.
		inputWait_.stage = "bridge queue Signal";
		hr = queue_->Signal(work->fence12.Get(), ++work->value);
		if (FAILED(hr)) { return Check(hr); }
		inputWait_.stage = "D3D11 bridge Wait";
		hr = WaitD3D11(Work::FrameGeneration);
		if (SUCCEEDED(hr)) { inputWait_.stage = "complete"; }
		return hr;
	}

}
