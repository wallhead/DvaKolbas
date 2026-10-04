#include "PerformanceTuning.h"
// Probe only: no frame telemetry or optional direct-output experiments. The
// compiled production DLSS backend uses its ordinary intermediate/copy route.
bool PerformanceTuning::IsRouteAllowed(Optimization) const{return false;}
void PerformanceTuning::MarkRouteEligible(Optimization,const char*){}
void PerformanceTuning::MarkRouteWaiting(Optimization,const char*){}
void PerformanceTuning::MarkRouteActive(Optimization){}
void PerformanceTuning::MarkRouteFallback(Optimization,const char*,bool){}
PerformanceTuning::D3D11StageTicket PerformanceTuning::BeginD3D11Stage(ID3D11DeviceContext*,D3D11Stage){return {};}
bool PerformanceTuning::EndD3D11Stage(ID3D11DeviceContext*,D3D11StageTicket){return false;}
