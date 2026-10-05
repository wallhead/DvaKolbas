#pragma once

#include <DirectXPackedVector.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>

namespace TheosRenderPipeline::NeuralRendering {

struct DiagnosticRegion { std::uint32_t x{},y{},width{},height{}; };
inline std::optional<DiagnosticRegion> DiagnosticReadbackRegion(std::uint32_t width,std::uint32_t height)
{
    if(!width || !height || width>16384 || height>16384)return std::nullopt;
    const auto w=(std::min)(width,512u),h=(std::min)(height,512u);
    return DiagnosticRegion{(width-w)/2,(height-h)/2,w,h};
}

// Read-only summaries of D3D11 staging rows. RGB is measured in source 8-bit
// code values, so a sign change is visible without guessing a transfer curve.
struct ColorPairStats {
    std::uint64_t samples{};
    std::array<double,3> before{},after{},signedChange{},absoluteChange{};
};

inline std::optional<ColorPairStats> CompareRgba8(const std::uint8_t* before,const std::uint8_t* after,
    std::size_t beforePitch,std::size_t afterPitch,std::uint32_t width,std::uint32_t height,std::uint32_t stride=8)
{
    if(!before||!after||!width||!height||!stride||width>16384||height>16384||
        beforePitch<std::size_t(width)*4||afterPitch<std::size_t(width)*4)return std::nullopt;
    ColorPairStats s;
    for(std::uint32_t y=0;y<height;y+=stride){
        const auto* oldRow=before+std::size_t(y)*beforePitch;
        const auto* newRow=after+std::size_t(y)*afterPitch;
        for(std::uint32_t x=0;x<width;x+=stride){
            ++s.samples;
            for(unsigned channel=0;channel<3;++channel){
                const auto oldValue=double(oldRow[std::size_t(x)*4+channel]);
                const auto newValue=double(newRow[std::size_t(x)*4+channel]);
                s.before[channel]+=oldValue;s.after[channel]+=newValue;
                s.signedChange[channel]+=newValue-oldValue;
                s.absoluteChange[channel]+=std::abs(newValue-oldValue);
            }
        }
    }
    for(unsigned channel=0;channel<3;++channel){
        s.before[channel]/=s.samples;s.after[channel]/=s.samples;
        s.signedChange[channel]/=s.samples;s.absoluteChange[channel]/=s.samples;
    }
    return s;
}

struct MotionStats {
    std::uint64_t samples{},nonFinite{},zeroVectors{},outsideUv{};
    double meanXpixels{},meanYpixels{},meanMagnitudePixels{},maxMagnitudePixels{};
};

inline std::optional<MotionStats> SummarizeMotionRg16(const std::uint8_t* data,std::size_t pitch,
    std::uint32_t width,std::uint32_t height,double scaleX,double scaleY,std::uint32_t stride=8)
{
    if(!data||!width||!height||!stride||width>16384||height>16384||pitch<std::size_t(width)*4||
        !std::isfinite(scaleX)||!std::isfinite(scaleY)||!scaleX||!scaleY)return std::nullopt;
    MotionStats s;
    for(std::uint32_t y=0;y<height;y+=stride){
        const auto* row=data+std::size_t(y)*pitch;
        for(std::uint32_t x=0;x<width;x+=stride){
            // Read unaligned half words safely; mapped row pitch is device chosen.
            const auto index=std::size_t(x)*4;
            const auto rawX=std::uint16_t(std::uint16_t(row[index])|(std::uint16_t(row[index+1])<<8));
            const auto rawY=std::uint16_t(std::uint16_t(row[index+2])|(std::uint16_t(row[index+3])<<8));
            const auto vx=double(DirectX::PackedVector::XMConvertHalfToFloat(rawX));
            const auto vy=double(DirectX::PackedVector::XMConvertHalfToFloat(rawY));
            if(!std::isfinite(vx)||!std::isfinite(vy)){++s.nonFinite;continue;}
            if(vx==0&&vy==0)++s.zeroVectors;
            if(std::abs(vx)>1||std::abs(vy)>1)++s.outsideUv;
            const auto px=vx*scaleX,py=vy*scaleY;
            const auto magnitude=std::hypot(px,py);
            s.meanXpixels+=px;s.meanYpixels+=py;s.meanMagnitudePixels+=magnitude;
            s.maxMagnitudePixels=(std::max)(s.maxMagnitudePixels,magnitude);++s.samples;
        }
    }
    if(s.samples){s.meanXpixels/=s.samples;s.meanYpixels/=s.samples;s.meanMagnitudePixels/=s.samples;}
    return s;
}

}
