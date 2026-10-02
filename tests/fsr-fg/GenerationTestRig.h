#pragma once
#include "../InteropTestRig.h"
#include "Upscaling/FSRGenerationParameters.h"
namespace GenerationFixture
{
    using namespace TheosRenderPipeline::Upscaling;
    using InteropFixture::Require;
    using InteropFixture::Check;
    using Microsoft::WRL::ComPtr;
    inline ComPtr<ID3D12Resource> Texture(ID3D12Device* device, Extent size, DXGI_FORMAT format)
    {
        D3D12_HEAP_PROPERTIES heap{}; heap.Type = D3D12_HEAP_TYPE_DEFAULT;
        D3D12_RESOURCE_DESC desc{}; desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        desc.Width = size.width; desc.Height = size.height; desc.DepthOrArraySize = desc.MipLevels = 1;
        desc.Format = format; desc.SampleDesc.Count = 1; desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        ComPtr<ID3D12Resource> result;
        Check(device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&result)), "FG fixture texture");
        return result;
    }
    struct Rig : InteropFixture::Rig
    {
        ComPtr<ID3D12CommandAllocator> allocator; ComPtr<ID3D12GraphicsCommandList> list;
        std::array<ComPtr<ID3D12Resource>, 4> owned;
        FsrGenerationResources resources; FsrGenerationLimits limits;
        UpscaleFrame frame;
        Rig()
        {
            limits.render = {64,64}; limits.display = {128,128}; limits.format = DXGI_FORMAT_R8G8B8A8_UNORM;
            owned[0] = Texture(device12.Get(), limits.display, limits.format);
            owned[1] = Texture(device12.Get(), limits.render, DXGI_FORMAT_R32_FLOAT);
            owned[2] = Texture(device12.Get(), limits.render, DXGI_FORMAT_R16G16_FLOAT);
            owned[3] = Texture(device12.Get(), limits.display, limits.format);
            resources = {owned[0].Get(), owned[1].Get(), owned[2].Get(), owned[3].Get(), ColorEncoding::SRGB};
            Check(device12->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator)), "FG allocator");
            Check(device12->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator.Get(), nullptr, IID_PPV_ARGS(&list)), "FG list");
            frame.backend = BackendKind::Fsr; frame.sourceId = 41; frame.deltaMilliseconds = 16.6667f;
            frame.render = frame.subrect = limits.render; frame.display = limits.display;
            frame.depthFormat = DXGI_FORMAT_R32_FLOAT; frame.motionFormat = DXGI_FORMAT_R16G16_FLOAT;
            frame.camera.identity = 7; frame.camera.nearDistance = 10; frame.camera.farDistance = 10000;
            frame.camera.verticalFovRadians = 1.04719755f; frame.camera.worldUnitsToMeters = .0142875f;
            frame.camera.position = {100, 200, 300};
            frame.camera.view = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
            frame.camera.projection = {1,0,0,0, 0,1.7320508f,0,0, 0,0,1.001001f,1, 0,0,-10.01001f,0};
            frame.motionConvention = {64,64,true,false}; frame.jitterX = .125f; frame.jitterY = -.25f;
        }
    };
}
