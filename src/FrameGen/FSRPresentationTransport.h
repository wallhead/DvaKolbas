#pragma once
#include "Graphics/D3D11D3D12Interop.h"
#include "Upscaling/FSRGenerationParameters.h"
#include <memory>
namespace TheosRenderPipeline
{
    // Render-thread owner. The bridge's FrameGeneration fence retires guides;
    // its SwapChain fence retires D3D11 scene/UI copy sources. Neither retires
    // SDK asynchronous presents. Publication resources enter/leave COMMON.
    class FsrPresentationTransport
    {
    public:
        FsrPresentationTransport();~FsrPresentationTransport();
        FsrPresentationTransport(const FsrPresentationTransport&)=delete;
        FsrPresentationTransport& operator=(const FsrPresentationTransport&)=delete;
        HRESULT Initialize(std::shared_ptr<Graphics::D3D11D3D12Interop>,Upscaling::Extent);
        HRESULT WaitBeforeProducer();
        HRESULT Upload(ID3D11Texture2D* scene,Upscaling::ColorEncoding,ID3D11Texture2D* ui,
            ID3D11ShaderResourceView* overlay,bool uiComplete,std::uint64_t sourceId);
        HRESULT PublishTo(ID3D12Resource* applicationBackbuffer);
        // Call only after the actual SDK UI registration succeeds. The first
        // stage always enables SDK internal UI buffering.
        HRESULT MarkUiRegistered();
        HRESULT NotifyPresentReturned(HRESULT);
        // Submit the already-recorded Prepare list, signaling before Present.
        HRESULT RecordPrepareRetirement();
        // Lifecycle proof provided only after actual UI unregister AND SDK
        // WaitForPresents succeed. Stops admissions; failure keeps all owners.
        HRESULT AcknowledgeSdkRetirement(HRESULT);
        HRESULT DrainForRetirement();
        HRESULT Retire();
        ID3D11Texture2D* SceneTarget11() const;
        const Upscaling::FsrGenerationResources& Resources() const;
    private:
        struct State;std::unique_ptr<State> state_;
    };
}
