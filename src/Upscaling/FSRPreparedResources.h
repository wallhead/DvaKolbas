#pragma once
#include "UpscalerBackend.h"
#include <d3d11.h>
#include <d3d12.h>
namespace TheosRenderPipeline::Upscaling
{
    enum class FsrResourceRole { Color, Depth, Motion, Output };
    inline D3D11_TEXTURE2D_DESC FsrPreparedTextureDesc(FsrResourceRole role,Extent size)
    {
        D3D11_TEXTURE2D_DESC desc{};
        desc.Width=size.width;desc.Height=size.height;desc.MipLevels=desc.ArraySize=1;desc.SampleDesc.Count=1;
        desc.Usage=D3D11_USAGE_DEFAULT;
        // RTV capability is needed for the tested D3D12 -> D3D11 shared-open
        // route, including copy-only motion and UAV-produced depth/output.
        desc.BindFlags=D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE;
        desc.Format=role==FsrResourceRole::Depth?DXGI_FORMAT_R32_FLOAT:
            role==FsrResourceRole::Motion?DXGI_FORMAT_R16G16_FLOAT:DXGI_FORMAT_R16G16B16A16_FLOAT;
        if(role==FsrResourceRole::Depth || role==FsrResourceRole::Output)desc.BindFlags|=D3D11_BIND_UNORDERED_ACCESS;
        return desc;
    }
    inline bool FsrPreparedFormatSupported(FsrResourceRole role,const D3D12_FEATURE_DATA_FORMAT_SUPPORT& support)
    {
        return support.Format==FsrPreparedTextureDesc(role,{1,1}).Format &&
            (support.Support1&D3D12_FORMAT_SUPPORT1_SHADER_LOAD) &&
            ((role!=FsrResourceRole::Depth && role!=FsrResourceRole::Output) || (support.Support2&D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE));
    }
}
