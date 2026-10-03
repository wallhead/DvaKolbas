#pragma once
#include "FSRUpscaler.h"
#include "FSRColorConversion.h"
#include "FSRHistoryPolicy.h"
#include <memory>
namespace TheosRenderPipeline::Upscaling
{
    enum class FsrCpuPhase { Total, PrepareColor, PrepareGuides, ProducerSignal, Begin, Record, Submit, Delivery, Count };
    enum class FsrGpuPhase { Prepare, Dispatch, Delivery };
    // Optional external diagnostics. The serialized caller retains this observer
    // through adapter/bridge retirement. It must never wait, Flush or alter work.
    // FSR itself has no dependency on the NR library or vendor runtime.
    class FsrPerformanceObserver {
    public:
        virtual ~FsrPerformanceObserver()=default;
        virtual void Cpu(FsrCpuPhase,bool begin)=0;
        virtual void Begin11(ID3D11DeviceContext*,uint64_t source)=0;
        virtual void Stamp11(ID3D11DeviceContext*,FsrGpuPhase,bool begin)=0;
        virtual void End11(ID3D11DeviceContext*)=0;
        virtual void Begin12(ID3D12GraphicsCommandList*,uint64_t source)=0;
        virtual void Stamp12(ID3D12GraphicsCommandList*,FsrGpuPhase,bool begin)=0;
        virtual void Resolve12(ID3D12GraphicsCommandList*)=0;
        virtual void DiscardUnsubmitted12()=0;
    };
    class FsrFrameAdapter final
    {
    public:
        FsrFrameAdapter(FsrUpscaler&,std::shared_ptr<Graphics::D3D11D3D12Interop>,GpuFrameResources,
            ID3D11Texture2D* color,ID3D11Texture2D* depth,ID3D11Texture2D* motion,ID3D11Texture2D* output,ColorEncoding handoffEncoding,FsrPerformanceObserver* = nullptr);
        ~FsrFrameAdapter();FsrFrameAdapter(const FsrFrameAdapter&)=delete;FsrFrameAdapter& operator=(const FsrFrameAdapter&)=delete;
        Result<UpscaleOutcome> Evaluate(const UpscaleFrame&);
        Result<UpscaleOutcome> Spatial(const UpscaleFrame&);
        void InvalidateHistory();
        const RuntimeError* LastError()const;
        bool LastTemporalReset()const;
    private:
        struct State;std::unique_ptr<State> state_;
    };
}
