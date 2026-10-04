#include "PostUpscale.h"
namespace TheosRenderPipeline::NeuralRendering {
Result<void> PostUpscale::Initialize(std::shared_ptr<RuntimeOwner> owner,ID3D11Device* device,
    const StageContract& contract,unsigned preset,PerformanceMetrics* metrics,ColorDomain domain){
    domain_=domain;
    return bridge_.Initialize(std::move(owner),device,contract,preset,metrics,domain,Placement::After);
}
Result<BeforeResult> PostUpscale::Evaluate(const PostSrInput& input,const SettingsSnapshot& settings){
    if(settings.enabled){
        if(settings.placement!=Placement::After)
            return std::unexpected(Error{ErrorKind::Unsupported,0,"NR post-upscale adapter requires After placement"});
        const auto valid=ValidatePostSrSourceContract(input.source);if(!valid)return std::unexpected(valid.error());
        const auto& r=input.resources;const auto& m=input.source;
        if(m.colorDomain!=domain_ || r.colorDomain!=m.colorDomain || r.epoch!=m.epoch || r.sourceId!=m.sourceId ||
            r.previousSourceId!=m.previousSourceId || r.guideEpoch!=m.guideEpoch || r.guideSourceId!=m.guideSourceId ||
            r.presentationTime!=m.sourceTime || r.colorExtent!=m.color || r.guideExtent!=m.guides ||
            r.motionScaleX!=valid->motionScaleX || r.motionScaleY!=valid->motionScaleY || r.depthInverted!=m.depthInverted)
            return std::unexpected(Error{ErrorKind::InvalidInput,0,"NR post-upscale metadata does not bind the actual real source"});
        D3D11_TEXTURE2D_DESC color{};if(r.color)r.color->GetDesc(&color);
        if(color.Format!=m.colorFormat)
            return std::unexpected(Error{ErrorKind::InvalidInput,0,"NR post-upscale source format differs from canonical metadata"});
    }
    return bridge_.Evaluate(input.resources,input.source.encoding,settings);
}
}
