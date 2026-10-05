#pragma once
#include <memory>
struct ID3D11DeviceContext;
// Single-threaded test fixture: one timestamp read reports S_FALSE once even
// though the real queue is retired. It never changes GPU submission or fences.
class D3D11QueryReadinessProbe {
public:
    explicit D3D11QueryReadinessProbe(ID3D11DeviceContext*);
    ~D3D11QueryReadinessProbe();
    bool Installed() const;
    unsigned CompletedPhaseReads() const;
    bool DelayedOneRead() const;
    struct State;
private:
    std::unique_ptr<State> state_;
};
