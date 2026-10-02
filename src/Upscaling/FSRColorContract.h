#pragma once
#include "UpscalerBackend.h"
namespace TheosRenderPipeline::Upscaling
{
    constexpr bool IsKnownColorEncoding(ColorEncoding value)
    {
        return value==ColorEncoding::Linear || value==ColorEncoding::Gamma22 || value==ColorEncoding::SRGB;
    }
    constexpr const char* ColorEncodingName(ColorEncoding value)
    {
        switch(value) {
        case ColorEncoding::Unknown:return "Unknown";
        case ColorEncoding::Linear:return "Linear";
        case ColorEncoding::Gamma22:return "Gamma22";
        case ColorEncoding::SRGB:return "SRGB";
        }
        return "Invalid";
    }
    constexpr bool SupportsFsrHandoffFormat(DXGI_FORMAT format)
    {
        return format==DXGI_FORMAT_R8G8B8A8_UNORM || format==DXGI_FORMAT_B8G8R8A8_UNORM ||
            format==DXGI_FORMAT_R16G16B16A16_FLOAT || format==DXGI_FORMAT_R32G32B32A32_FLOAT;
    }
    inline Result<void> ValidateFsrHandoff(DXGI_FORMAT format,ColorEncoding encoding)
    {
        if(!SupportsFsrHandoffFormat(format))return std::unexpected(RuntimeError{ErrorKind::InvalidInput,0,
            "FSR unsupported source/output DXGI_FORMAT="+std::to_string(static_cast<unsigned>(format))+"; reduced targets were not prepared"});
        if(!IsKnownColorEncoding(encoding))return std::unexpected(RuntimeError{ErrorKind::InvalidInput,0,
            "FSR source encoding is unknown; configure [FSR] SourceColorEncoding as Linear, Gamma22 or SRGB after checking the producer"});
        return {};
    }
}
