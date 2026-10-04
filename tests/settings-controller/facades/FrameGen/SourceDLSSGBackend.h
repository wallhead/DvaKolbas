#pragma once
#include "FrameGen/SourceDLSSGSettings.h"
#include <sl_reflex.h>
namespace TheosRenderPipeline::SourceDLSSG {
// The vendor boundary is a recording facade. This test cannot qualify vendor FG.
struct NeuralOptions {
    bool enabled{},beforeUpscaling{};std::string runtimePath;
    NeuralRendering::Tuning tuning;NeuralRendering::Reconstruction reconstruction;
    NeuralRendering::SecondPassSettings secondPass;int passes{1};NeuralRendering::CombatSettings combat;
};
inline bool NeuralRuntimePresent(const std::string&){return true;}
class Backend {
    NeuralOptions options_;
public:
    bool ready{true};unsigned configurations{};
    struct State{bool failed{};}state;
    static Backend& Get(){static Backend v;return v;}
    bool Ready()const{return ready;}
    const auto& NeuralState()const{return state;}
    auto NeuralConfiguration()const{return options_;}
    void ConfigureReflex(sl::ReflexMode){++configurations;}
    void ConfigureUIRecomposition(bool){++configurations;}
    void ConfigureOutputFPSLimit(int){++configurations;}
    void ConfigureGeneration(GenerationRequest){++configurations;}
    void ConfigureHDROutput(HDROutput::Settings){++configurations;}
    void ConfigureNeuralRendering(NeuralOptions v){++configurations;options_=std::move(v);}
};
}
