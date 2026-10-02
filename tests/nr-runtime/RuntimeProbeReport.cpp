#include "RuntimeProbeReport.h"
#include <algorithm>
#include <stdexcept>
namespace NrRuntimeResearch {
std::vector<uint16_t> RgbForTemporalHash(std::span<const uint16_t> rgba) {
    if(rgba.size()%4)throw std::invalid_argument("packed RGBA size is invalid");
    std::vector<uint16_t> rgb;rgb.reserve(rgba.size()/4*3);
    for(size_t i=0;i<rgba.size();i+=4)rgb.insert(rgb.end(),rgba.begin()+i,rgba.begin()+i+3);
    return rgb;
}
bool AllRgbOverwritten(std::span<const uint16_t,4> rgba,std::span<const uint16_t,4> sentinel) {
    return rgba[0]!=sentinel[0] && rgba[1]!=sentinel[1] && rgba[2]!=sentinel[2];
}
std::vector<std::string> Validate(const ProbeReport& r) {
    std::vector<std::string> issues;
    const auto hash=[](const std::string& s){return s.size()==64 && std::ranges::all_of(s,[](char c){
        return (c>='0'&&c<='9')||(c>='a'&&c<='f');});};
    if(!r.failure.empty())issues.push_back(r.failure);
    if(r.profile.empty() || !hash(r.runtimeSha256) || !hash(r.coreSha256) ||
        !r.runtimeHeldAndMatched || !r.coreHeldAndMatched)issues.push_back("held runtime/core identity incomplete");
    if(r.init!=1 || r.create!=1 || r.evaluate!=1 || r.release!=1 || r.destroyParameters!=1 || r.shutdown!=1)
        issues.push_back("native operation did not succeed");
    if(r.requestedFrames<2 || r.readbackFrames!=r.requestedFrames || r.distinctOutputHashes<2 ||
        r.distinctOutputHashes>r.readbackFrames || r.spatiallyVariedFrames!=r.readbackFrames)
        issues.push_back("changing spatial output frame coverage incomplete");
    if(!r.outputPixels || r.finitePixels!=r.outputPixels || r.overwrittenPixels!=r.outputPixels ||
        !r.changedFromInputPixels || r.changedFromInputPixels>r.outputPixels)issues.push_back("NR pixels are unqualified");
    if(!r.outputReadersRetired || r.allocations!=r.releases || (r.shimRequested && !r.shimRestored))
        issues.push_back("runtime/output retirement incomplete");
    return issues;
}
}
