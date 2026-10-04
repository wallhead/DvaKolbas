#pragma once
#include "FrameGen/D3D11ContextIsolation.h"
#include <d3d11.h>
#include <wrl/client.h>

namespace TheosRenderPipeline::NeuralRendering {
class PerformanceMetrics;
// SDR linear FP16 only. Remove broad NR color changes using the original
// source as the edge guide; retain the finer NR residual and source alpha.
// Views borrow a bridge slot's images. Its reader fence must cover Draw.
class StableColorResolve {
public:
    struct Views {
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> original,nr;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> output;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> originalImage,nrImage,outputImage;
        UINT width{},height{};
    };
    HRESULT Initialize(ID3D11Device*);
    HRESULT Prepare(Views&,ID3D11Texture2D* original,ID3D11Texture2D* nr,ID3D11Texture2D* output,PerformanceMetrics* = nullptr);
    HRESULT Draw(ID3D11DeviceContext*,const Views&);
private:
    Microsoft::WRL::ComPtr<ID3D11Device> device_;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_;
    D3D11ContextIsolation isolation_;
};
}
