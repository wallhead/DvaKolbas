#pragma once
#include <optional>
#include <string_view>
namespace TheosRenderPipeline::NeuralRendering {
enum class FsrNrRoute { Bypassed,PreparedLinear,EncodedForReShadeBefore,EncodedForDiagnostic,Encoded };
constexpr std::string_view FsrNrRouteName(FsrNrRoute route){
    switch(route){
    case FsrNrRoute::Bypassed:return "Bypassed";
    case FsrNrRoute::PreparedLinear:return "PreparedLinear";
    case FsrNrRoute::EncodedForReShadeBefore:return "EncodedForReShadeBefore";
    case FsrNrRoute::EncodedForDiagnostic:return "EncodedForDiagnostic";
    default:return "Encoded";
    }
}
struct FsrRouteObservation {
    FsrNrRoute route{};bool reshadeBefore{};
    bool operator==(const FsrRouteObservation&)const=default;
};
// Observe actual completed NR admission/lease creation, not a requested mode.
// Stable frames never notify; retirement resets the next startup notification.
class FsrRouteDiagnostics {
public:
    std::optional<FsrRouteObservation> Observe(bool evaluated,bool preparedLease,bool reshadeBefore,bool diagnosticCapture){
        const auto route=!evaluated?FsrNrRoute::Bypassed:preparedLease?FsrNrRoute::PreparedLinear:
            diagnosticCapture?FsrNrRoute::EncodedForDiagnostic:reshadeBefore?FsrNrRoute::EncodedForReShadeBefore:FsrNrRoute::Encoded;
        const FsrRouteObservation observation{route,reshadeBefore};
        if(last_&&*last_==observation)return std::nullopt;
        last_=observation;return observation;
    }
    void Reset(){last_.reset();}
private:
    std::optional<FsrRouteObservation> last_;
};
}
