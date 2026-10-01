#pragma once
#include "UpscalerBackend.h"
#include "FrameGen/D3D11ContextIsolation.h"
#include <d3d11.h>
#include <wrl/client.h>
namespace TheosRenderPipeline::Upscaling
{
    enum class ColorEncoding { Unknown,Linear,Gamma22,SRGB };
    Result<float> ConvertColorChannel(float,ColorEncoding source,ColorEncoding destination);
    class FsrColorConverter
    {
    public:
        HRESULT Convert(ID3D11DeviceContext*,ID3D11Texture2D* input,ID3D11Texture2D* output,ColorEncoding source,ColorEncoding destination);
    private:
        template<class T> using Ptr=Microsoft::WRL::ComPtr<T>;
        HRESULT Initialize(ID3D11Device*);
        Ptr<ID3D11Device> device_;Ptr<ID3D11VertexShader> vertex_;Ptr<ID3D11PixelShader> pixel_;
        Ptr<ID3D11SamplerState> sampler_;Ptr<ID3D11Buffer> constants_;
        Ptr<ID3D11Texture2D> input_,output_;Ptr<ID3D11ShaderResourceView> srv_;Ptr<ID3D11RenderTargetView> rtv_;
        D3D11ContextIsolation isolation_;
    };
}
