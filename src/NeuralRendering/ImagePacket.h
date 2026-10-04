#pragma once
#include "RuntimeCatalog.h"
#include "FrameGen/NeuralRenderingTuning.h"
#include "FrameGen/NeuralRenderingReconstruction.h"
#include <d3d12.h>
#include <wrl/client.h>
#include <optional>

namespace TheosRenderPipeline::NeuralRendering {
enum class Placement { Before, After };
enum class ImageKind { Real, Generated };
enum class ColorDomain { Unknown, Linear, SdrBytes };
constexpr DXGI_FORMAT NrColorFormat(ColorDomain domain){
    return domain==ColorDomain::Linear?DXGI_FORMAT_R16G16B16A16_FLOAT:
        domain==ColorDomain::SdrBytes?DXGI_FORMAT_R8G8B8A8_UNORM:DXGI_FORMAT_UNKNOWN;
}
enum class GuideOrigin { Unknown, RealSource, GeneratedProvider, ReconstructedPair };
struct ImageExtent {
    uint32_t width{},height{};
    bool operator==(const ImageExtent&)const=default;
};
struct SettingsSnapshot {
    uint64_t revision{};
    bool enabled{};
    bool stableColors{true};
    Placement placement{Placement::Before};
    Tuning tuning;
    Reconstruction reconstruction;
};
struct ImagePacket {
    // Retaining these references does not prove GPU completion. Stage tickets
    // must retain the whole packet until all submitted readers retire.
    Microsoft::WRL::ComPtr<ID3D12Resource> color,output,depth,motion,ui;
    Microsoft::WRL::ComPtr<ID3D12Fence> producerFence;
    uint64_t producerFenceValue{};
    uint64_t epoch{},batchId{},imageId{},sourceId{},guideEpoch{},guideSourceId{};
    uint64_t previousSourceId{};
    ImageKind kind{ImageKind::Real};
    std::optional<double> interpolationFraction;
    double presentationTime{};
    ImageExtent colorExtent,guideExtent;
    ColorDomain colorDomain{ColorDomain::Unknown};
    GuideOrigin guideOrigin{GuideOrigin::Unknown};
    float motionScaleX{},motionScaleY{};
    bool reset{},depthInverted{};
    // Declared states are an adapter contract: D3D12 has no resource-state
    // getter. Validation checks ownership and shape, not the GPU's true state.
    D3D12_RESOURCE_STATES colorState{D3D12_RESOURCE_STATE_COMMON};
    D3D12_RESOURCE_STATES outputState{D3D12_RESOURCE_STATE_COMMON};
    D3D12_RESOURCE_STATES depthState{D3D12_RESOURCE_STATE_COMMON};
    D3D12_RESOURCE_STATES motionState{D3D12_RESOURCE_STATE_COMMON};
};
struct StageContract {
    Microsoft::WRL::ComPtr<ID3D12Device> device;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> queue;
    AdapterLuid adapterLuid;
    ImageExtent colorExtent,guideExtent;
};
// ReShade can forward CreateFence to its native device while resources and
// queues report the wrapper. Establish that native identity only through a
// private reference fence created by the retained host device, never through
// an incoming fence, adapter LUID, or caller-supplied alternate device.
class FenceDeviceIdentity {
public:
    Result<void> Initialize(ID3D12Device*);
    Result<void> Validate(ID3D12Fence*,ID3D12Device* expectedHost)const;
private:
    Microsoft::WRL::ComPtr<IUnknown> host_,native_;
    Microsoft::WRL::ComPtr<ID3D12Fence> reference_;
};
Result<void> ValidateImagePacket(ID3D12GraphicsCommandList*,const ImagePacket&,const StageContract&,
    const FenceDeviceIdentity* = nullptr);
// Only Stage can request admission of a pending dependency. Callers cannot
// weaken the public completion validator with a flag or an alternate fence.
class QueuedImageAdmission {
    friend class Stage;
    static Result<void> Validate(ID3D12GraphicsCommandList*,const ImagePacket&,const StageContract&,const FenceDeviceIdentity&);
};
}
