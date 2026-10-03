#include "NeuralRendering/TicketOwnership.h"
#include <cstdio>
#include <memory>
using TheosRenderPipeline::NeuralRendering::Detail::TicketOwnership;
int main(){
    alignas(TicketOwnership) unsigned char storage[sizeof(TicketOwnership)];
    auto* first=std::construct_at(reinterpret_cast<TicketOwnership*>(storage));
    auto stale=first->Seal();if(!first->Owns(stale))return 1;
    std::destroy_at(first);
    auto* replacement=std::construct_at(reinterpret_cast<TicketOwnership*>(storage));
    const bool rejects=!replacement->Owns(stale);
    const auto current=replacement->Seal();const bool accepts=replacement->Owns(current);
    std::destroy_at(replacement);
    std::printf("%s OldTicketRejectedAfterSameAddressOwnerReplacement\n",rejects?"PASS":"FAIL");
    return rejects&&accepts?0:1;
}
