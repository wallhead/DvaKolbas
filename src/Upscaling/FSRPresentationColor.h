#pragma once
#include "FSRColorContract.h"
#include "FrameGen/D3D11ContextIsolation.h"
#include <wrl/client.h>
namespace TheosRenderPipeline::Upscaling
{
    // Both inputs use the explicitly supplied native transfer and premultiplied
    // coverage. The output is premultiplied sRGB in a non-sRGB UNORM texture.
    class FsrPresentationUiConverter
    {
    public:
        HRESULT Convert(ID3D11DeviceContext*,ID3D11Texture2D* ui,ID3D11ShaderResourceView* overlay,
            ID3D11Texture2D* output,ColorEncoding source);
    private:
        template<class T> using Ptr=Microsoft::WRL::ComPtr<T>;
        HRESULT Initialize(ID3D11Device*);
        Ptr<ID3D11Device> device_;Ptr<ID3D11VertexShader> vertex_;Ptr<ID3D11PixelShader> pixel_;
        Ptr<ID3D11Buffer> constants_;Ptr<ID3D11Texture2D> input_,output_;
        Ptr<ID3D11ShaderResourceView> uiView_;Ptr<ID3D11RenderTargetView> outputView_;
        D3D11ContextIsolation isolation_;
    };
}
