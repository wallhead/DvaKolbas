#define CINTERFACE
#define D3D11_NO_HELPERS
#include <d3d11.h>
#include "D3D11QueryReadinessProbe.h"
#include <cstddef>
struct D3D11QueryReadinessProbe::State {
    ID3D11DeviceContext* context{};
    using GetData = HRESULT(STDMETHODCALLTYPE*)(ID3D11DeviceContext*, ID3D11Asynchronous*, void*, UINT, UINT);
    GetData original{};
    void** slot{};
    ID3D11Asynchronous *begin{}, *end{};
    unsigned calls{}, completedReads{};
    bool delayed{};
};
namespace {
D3D11QueryReadinessProbe::State* active{};
HRESULT STDMETHODCALLTYPE Read(ID3D11DeviceContext* context, ID3D11Asynchronous* query, void* out, UINT bytes, UINT flags) {
    auto& s = *active;
    if (context == s.context) {
        const auto call = s.calls++;
        if (call == 1) s.begin = query;
        if (call == 2) s.end = query;
        if (query == s.begin || query == s.end) ++s.completedReads;
        if (call == 3) { s.delayed = true; return S_FALSE; }
    }
    return s.original(context, query, out, bytes, flags);
}
bool Replace(void** slot, void* replacement) {
    DWORD old{}, ignored{};
    if (!VirtualProtect(slot, sizeof(void*), PAGE_READWRITE, &old)) return false;
    InterlockedExchangePointer(slot, replacement);
    return VirtualProtect(slot, sizeof(void*), old, &ignored) != FALSE;
}
}
D3D11QueryReadinessProbe::D3D11QueryReadinessProbe(ID3D11DeviceContext* context) : state_(std::make_unique<State>()) {
    auto& s = *state_;
    if (active || !context) return;
    s.context = context;
    s.slot = reinterpret_cast<void**>(const_cast<ID3D11DeviceContextVtbl*>(context->lpVtbl)) + offsetof(ID3D11DeviceContextVtbl, GetData) / sizeof(void*);
    s.original = reinterpret_cast<State::GetData>(*s.slot);
    active = &s;
    if (!Replace(s.slot, reinterpret_cast<void*>(&Read))) { active = nullptr; s.slot = nullptr; }
}
D3D11QueryReadinessProbe::~D3D11QueryReadinessProbe() {
    if (state_->slot) {
        if (!Replace(state_->slot, reinterpret_cast<void*>(state_->original))) ExitProcess(1);
        active = nullptr;
    }
}
bool D3D11QueryReadinessProbe::Installed() const { return state_->slot != nullptr; }
unsigned D3D11QueryReadinessProbe::CompletedPhaseReads() const { return state_->completedReads; }
bool D3D11QueryReadinessProbe::DelayedOneRead() const { return state_->delayed; }
