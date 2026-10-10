#pragma once
#include "Upscaling/UpscalerBackend.h"
#include "D3D11FrameCopy.h"
namespace TheosRenderPipeline
{
    // Freeze final real-scene metadata after SR/NR/effects. Guide textures are
    // borrowed until the Intel transport has copied and fence-sealed them.
    inline Upscaling::Result<Upscaling::UpscaleFrame> CompleteXessGenerationSource(
        Upscaling::UpscaleFrame frame,ID3D11Texture2D* scene,Upscaling::ColorEncoding encoding)
    {
        using namespace Upscaling;
        const auto invalid=[](const char* reason)->Result<UpscaleFrame> {
            return std::unexpected(RuntimeError{ErrorKind::InvalidInput,E_INVALIDARG,reason});
        };
        if(!scene || encoding==ColorEncoding::Unknown)return invalid("Intel final real scene requires an explicit SDR encoding");
        D3D11_TEXTURE2D_DESC output{};scene->GetDesc(&output);
        if(Extent{output.Width,output.Height}!=frame.display || output.SampleDesc.Count!=1 ||
            output.Format!=DXGI_FORMAT_R8G8B8A8_UNORM)return invalid("Intel final real scene does not match its fixed SDR display contract");
        Microsoft::WRL::ComPtr<ID3D11Device> producer;scene->GetDevice(&producer);
        frame.output=scene;frame.outputEncoding=encoding;frame.uiEncoding=ColorEncoding::SRGB;
        // Retain guide identity and measure its actual valid region. The owned
        // transport subsequently validates readability and copies packed depth.
        if(frame.depth && frame.motion) {
            D3D11_TEXTURE2D_DESC depth{},motion{};frame.depth->GetDesc(&depth);frame.motion->GetDesc(&motion);
            Microsoft::WRL::ComPtr<ID3D11Device> depthOwner,motionOwner;
            frame.depth->GetDevice(&depthOwner);frame.motion->GetDevice(&motionOwner);
            if(!D3D11FrameCopy::SameObject(producer.Get(),depthOwner.Get()) || !D3D11FrameCopy::SameObject(producer.Get(),motionOwner.Get()))
                return invalid("Intel final guide identity differs from the retained scene producer");
            frame.depthExtent={depth.Width,depth.Height};frame.motionExtent={motion.Width,motion.Height};
            frame.depthFormat=DXGI_FORMAT_R32_FLOAT;frame.motionFormat=motion.Format;
        }
        return frame;
    }
}
