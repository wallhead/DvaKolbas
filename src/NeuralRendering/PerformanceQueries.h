#pragma once
#include "PerformanceMetrics.h"
#include <d3d11.h>
#include <d3d12.h>
#include <wrl/client.h>
#include <memory>
namespace TheosRenderPipeline::NeuralRendering {
// Diagnostic commands only. Collect performs nonblocking completion checks;
// every issued query/resolve owner lives with its rendering owner until retire.
class PerformanceQueries {
public:
    explicit PerformanceQueries(PerformanceMetrics*);
    ~PerformanceQueries();
    HRESULT Initialize11(ID3D11Device*);
    HRESULT Initialize12(ID3D12Device*,ID3D12CommandQueue*);
    void Collect11(ID3D11DeviceContext*);
    void Collect12();
    void Begin11(ID3D11DeviceContext*,uint64_t source);
    void Stamp11(ID3D11DeviceContext*,GpuPhase,bool begin);
    void End11(ID3D11DeviceContext*);
    void Begin12(ID3D12GraphicsCommandList*,uint64_t source);
    void Stamp12(ID3D12GraphicsCommandList*,GpuPhase,bool begin);
    void Resolve12(ID3D12GraphicsCommandList*);
    void Submitted12(ID3D12Fence*,uint64_t value);
    // Only after the owning command list was successfully discarded without
    // ExecuteCommandLists. Failed/uncertain discard must retain this recording.
    void DiscardUnsubmitted12();
    uint64_t Dropped()const;
    bool Available11()const;
    bool Available12()const;
private:
    struct State;std::unique_ptr<State> state_;
};
}
