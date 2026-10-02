#pragma once
#include <array>
#include <cstdint>
#include <cmath>
#include <stdexcept>
#include <set>
#include <vector>
namespace NrPostFgResearch {
// Independent orthographic reference renderer. It takes geometry + camera poses,
// never source-pair guide buffers or a guide reconstruction kernel's output.
struct Box {
    double left{},top{},width{},height{},velocityX{},velocityY{};
    float depthMetres{1};
    std::array<float,4> color{1,1,1,1};
    unsigned id{1};
};
struct Scene {
    std::vector<Box> boxes;
    double cameraVelocityX{},cameraVelocityY{};
    std::array<float,4> background{.1f,.2f,.3f,1};
    float backgroundDepthMetres{10};
};
struct ReferencePixel {
    std::array<float,4> color{};
    float depthMetres{},currentToHistoryPixelsX{},currentToHistoryPixelsY{};
    unsigned objectId{};
};
struct ReferenceFrame {
    unsigned width{},height{};
    double time{},historyTime{};
    std::vector<ReferencePixel> pixels;
};
inline ReferenceFrame Rasterize(const Scene& scene,unsigned width,unsigned height,double time,double historyTime) {
    if(!width || !height || uint64_t(width)*height>16*1024*1024 || !std::isfinite(time) ||
        !std::isfinite(historyTime) || historyTime>time || !std::isfinite(scene.cameraVelocityX) ||
        !std::isfinite(scene.cameraVelocityY) || !std::isfinite(scene.backgroundDepthMetres) || scene.backgroundDepthMetres<=0)
        throw std::invalid_argument("reference extent/time/camera is invalid");
    std::set<unsigned> ids;
    for(const auto& box:scene.boxes){
        if(!box.id || !ids.insert(box.id).second || !std::isfinite(box.left) || !std::isfinite(box.top) ||
            !std::isfinite(box.width) || !std::isfinite(box.height) || box.width<=0 || box.height<=0 ||
            !std::isfinite(box.velocityX) || !std::isfinite(box.velocityY) ||
            !std::isfinite(box.depthMetres) || box.depthMetres<=0)
            throw std::invalid_argument("reference geometry is invalid");
    }
    ReferenceFrame frame{width,height,time,historyTime,std::vector<ReferencePixel>(size_t(width)*height)};
    const auto dt=time-historyTime;
    for(unsigned y=0;y<height;++y)for(unsigned x=0;x<width;++x){
        auto& p=frame.pixels[size_t(y)*width+x];p.color=scene.background;p.depthMetres=scene.backgroundDepthMetres;
        p.currentToHistoryPixelsX=float(scene.cameraVelocityX*dt);p.currentToHistoryPixelsY=float(scene.cameraVelocityY*dt);
        const double worldX=x+.5+scene.cameraVelocityX*time,worldY=y+.5+scene.cameraVelocityY*time;
        for(const auto& box:scene.boxes){
            const double left=box.left+box.velocityX*time,top=box.top+box.velocityY*time;
            if(worldX<left || worldX>=left+box.width || worldY<top || worldY>=top+box.height || box.depthMetres>=p.depthMetres)continue;
            p.objectId=box.id;p.color=box.color;p.depthMetres=box.depthMetres;
            p.currentToHistoryPixelsX=float((scene.cameraVelocityX-box.velocityX)*dt);
            p.currentToHistoryPixelsY=float((scene.cameraVelocityY-box.velocityY)*dt);
        }
    }
    return frame;
}
}
