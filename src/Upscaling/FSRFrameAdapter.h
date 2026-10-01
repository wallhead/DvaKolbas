#pragma once
#include "FSRUpscaler.h"
#include "FSRColorConversion.h"
#include "FSRHistoryPolicy.h"
#include <memory>
namespace TheosRenderPipeline::Upscaling
{
    class FsrFrameAdapter final
    {
    public:
        FsrFrameAdapter(FsrUpscaler&,std::shared_ptr<Graphics::D3D11D3D12Interop>,GpuFrameResources,
            ID3D11Texture2D* color,ID3D11Texture2D* depth,ID3D11Texture2D* motion,ID3D11Texture2D* output,ColorEncoding handoffEncoding);
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
