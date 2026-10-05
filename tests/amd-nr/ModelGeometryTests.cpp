#include "NeuralRendering/Amd/ModelGeometry.h"
#include "TestSupport.h"
#include <limits>
using namespace TheosRenderPipeline::NeuralRendering::Amd;
using AmdNrTest::Require;
int main() {
    struct Example { Extent in,processing,deep; };
    const Example examples[]{ {{1280,720},{1280,832},{20,16}}, {{1600,900},{1600,960},{28,16}}, {{1920,1080},{1920,1088},{32,20}}, {{2560,1440},{2560,1472},{40,24}}, {{3840,2160},{3840,2176},{60,36}} };
    for(const auto& e:examples) {
        auto g=MakeGeometry(e.in);Require(bool(g),"reference geometry succeeds");
        Require(g->input==e.in && g->processing==e.processing && g->levels[5].extent==e.deep,"reference geometry fields");
        for(unsigned i=0;i<6;++i) Require(g->levels[i].channels==(32u<<i),"channel sequence");
    }
    Require(MakeGeometry({1024,1024})->processing==Extent{1024,1088},"extra height");
    Require(MakeGeometry({1024,1024},ExtentMode::Default,true)->processing==Extent{1024,1024},"disable extra height");
    Require(MakeGeometry({1,7})->processing==Extent{320,320},"minimum extent");
    const auto s8=MakeGeometry({17,9},ExtentMode::Step8).value();
    Require(s8.processing==Extent{24,16} && s8.levels[0].extent==Extent{12,8} && s8.levels[1].extent==Extent{8,4} && s8.levels[5].extent==Extent{4,4},"Step8 non-square levels");
    const auto s128=MakeGeometry({129,1},ExtentMode::Step128).value();
    Require(s128.processing==Extent{256,128},"Step128 processing");
    for(unsigned i=0;i<6;++i) Require(s128.levels[i].extent==Extent{256u>>(i+1),128u>>(i+1)},"Step128 exact levels");
    for(auto mode:{ExtentMode::Default,ExtentMode::Step8,ExtentMode::Step128}) {
        Require(!MakeGeometry({0,1},mode) && !MakeGeometry({1,0},mode),"zero dimension rejected");
        for(auto n:{0xffffffffu,0x80000000u,0x7fffffffu}) Require(!MakeGeometry({n,1},mode) && !MakeGeometry({1,n},mode),"overflow and rounding carry rejected");
    }
    Require(!MakeGeometry({1,1},static_cast<ExtentMode>(99)),"invalid mode");
    Require(bool(MakeGeometry({0x7fffffc0,1},ExtentMode::Default)),"largest rounded signed extent valid");
    std::puts("PASS: AMD NR geometry");
}
