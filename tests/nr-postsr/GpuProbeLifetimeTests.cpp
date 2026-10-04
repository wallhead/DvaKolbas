#include "GpuProbeLifetime.h"
#include <cstdio>
#include <stdexcept>
struct Resources{std::shared_ptr<int> reader;};
int main(){unsigned failures{};
    for(unsigned mode=0;mode<3;++mode){auto object=std::make_shared<int>(42);std::weak_ptr<int> tracked=object;unsigned calls{};
        {GpuProbeLifetime<Resources> guard(std::make_unique<Resources>(Resources{std::move(object)}),[&]{++calls;if(mode==2)throw std::runtime_error("uncertain submit");return mode==0;});
            const bool result=guard.Retire();if(result!=(mode==0)){++failures;std::puts("FAIL ConfirmedRetirementOnly");}
            if(tracked.expired()!=(mode==0)){++failures;std::puts("FAIL UncertainReadersMustStayOwned");}
            if(guard.Retire()!=result||calls!=1){++failures;std::puts("FAIL FailedRetirementIsNeverRetried");}}
        if(calls!=1||tracked.expired()!=(mode==0)){++failures;std::puts("FAIL OwnershipSurvivesGuardUnwind");}}
    std::printf("PROBE_LIFETIME failures=%u confirmed/refused/throwing retirement; no failed retry\n",failures);return failures?1:0;
}
