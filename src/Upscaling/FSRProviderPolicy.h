#pragma once
#include "UpscalerBackend.h"
#include <span>
#include <cctype>
#include <charconv>
#include <optional>
namespace TheosRenderPipeline::Upscaling
{
    // Version names come from the device's catalog. IDs remain opaque and are
    // passed unchanged to sizing, creation and actual-provider verification.
    inline std::optional<std::array<unsigned,3>> FsrProviderVersion(const ProviderInfo& provider)
    {
        const auto& name=provider.name;
        for(std::size_t start=0;start<name.size();++start) {
            if(!std::isdigit(static_cast<unsigned char>(name[start])) ||
                (start && (std::isdigit(static_cast<unsigned char>(name[start-1])) || name[start-1]=='.')))continue;
            std::array<unsigned,3> version{};const char* cursor=name.data()+start;const char* end=name.data()+name.size();bool valid=true;
            for(unsigned part=0;part<3;++part) {
                const auto parsed=std::from_chars(cursor,end,version[part]);
                if(parsed.ec!=std::errc{}){valid=false;break;}
                cursor=parsed.ptr;
                if(part<2){if(cursor==end || *cursor!='.'){valid=false;break;}++cursor;}
            }
            if(valid && (cursor==end || (*cursor!='.' && !std::isdigit(static_cast<unsigned char>(*cursor)))))return version;
        }
        return {};
    }
    inline bool IsFsr4Provider(const ProviderInfo& provider)
    { const auto version=FsrProviderVersion(provider);return version && (*version)[0]==4; }

    inline Result<ProviderInfo> SelectProvider(std::span<const ProviderInfo> providers, ProviderPolicy policy,
        std::uint32_t adapterVendorId=0)
    {
        if(!ValidProviderPolicy(policy))return std::unexpected(RuntimeError{ErrorKind::InvalidInput,0,"Invalid FSR provider policy"});
        const ProviderInfo* analytical{};const ProviderInfo* ml{};
        std::array<unsigned,3> newest{};
        for(const auto& provider:providers) {
            if(!provider.id)continue;
            const auto version=FsrProviderVersion(provider);if(!version)continue;
            if(*version==std::array<unsigned,3>{3,1,5} && !analytical)analytical=&provider;
            if((*version)[0]==4 && (!adapterVendorId || adapterVendorId==0x1002) && (!ml || *version>newest)) {
                ml=&provider;newest=*version;
            }
        }
        if(policy!=ProviderPolicy::Analytical && ml)return *ml;
        if(policy!=ProviderPolicy::MachineLearning && analytical)return *analytical;
        return std::unexpected(RuntimeError{ErrorKind::NoProvider, 0, "Requested FSR provider is unavailable; no version ID was fabricated"});
    }
}
