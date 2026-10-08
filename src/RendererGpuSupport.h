#pragma once
#include "NeuralRendering/RuntimeCatalog.h"
#include "GpuProductName.h"
#include <string_view>
namespace TheosRenderPipeline {
enum class RendererGpuChoice { NvidiaRtx, FsrOnly, Unsupported };
// Capabilities refer to the actual rendering device: D3D11 >= 11_0 and
// D3D12 device creation at 12_0, as required by both presentation hosts.
inline RendererGpuChoice ClassifyRendererGpu(const NeuralRendering::AdapterIdentity& adapter,
    std::wstring_view name, bool d3d11Supported = true, bool d3d12Supported = true)
{
    if (adapter.software || !adapter.luid.Valid() || !d3d11Supported || !d3d12Supported)
        return RendererGpuChoice::Unsupported;
    if (adapter.vendorId != 0x10de) return RendererGpuChoice::FsrOnly;
    if (adapter.architecture.queried) {
        const auto& evidence = adapter.architecture;
        if (evidence.luid != adapter.luid) return RendererGpuChoice::FsrOnly;
        if (!evidence.mappingAmbiguous)
            return evidence.rtxProduct && evidence.id >= 0x160
                ? RendererGpuChoice::NvidiaRtx : RendererGpuChoice::FsrOnly;
        // Linked/ambiguous NR mapping is not a negative RTX product report.
        // Keep NR unavailable, but allow this render adapter's PCI/name proof.
    }
    const auto family = NeuralRendering::ClassifyGpu(adapter.vendorId, adapter.deviceId, false);
    if (family == NeuralRendering::GpuFamily::Rtx20 || family == NeuralRendering::GpuFamily::Rtx30 ||
        family == NeuralRendering::GpuFamily::Rtx40 || family == NeuralRendering::GpuFamily::Rtx50)
        return RendererGpuChoice::NvidiaRtx;
    // DXGI supplies this name for the same render LUID. Covers laptops and
    // workstation RTX cards when public NVAPI discovery is unavailable.
    if (IsRtxProductName(name))
        return RendererGpuChoice::NvidiaRtx;
    return RendererGpuChoice::FsrOnly;
}
}
