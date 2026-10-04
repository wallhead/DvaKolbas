#pragma once
#include <functional>
#include <memory>
// Test-only ownership capsule for resources referenced by submitted lists.
// Quarantine on uncertainty; never repeat a failed retirement callback.
template<class Resources> class GpuProbeLifetime {
public:
    GpuProbeLifetime(std::unique_ptr<Resources> resources,std::function<bool()> retire):resources_(std::move(resources)),retire_(std::move(retire)){}
    ~GpuProbeLifetime(){if(!attempted_)Retire();}
    bool Retire() noexcept {
        if(attempted_)return retired_;attempted_=true;
        try{retired_=retire_();}catch(...){retired_=false;}
        if(retired_)resources_.reset();else (void)resources_.release();
        return retired_;
    }
private:
    std::unique_ptr<Resources> resources_;std::function<bool()> retire_;bool attempted_{},retired_{};
};
