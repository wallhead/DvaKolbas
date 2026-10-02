#pragma once
#include <string_view>
#include <cstddef>

// The fixture compiles the production recorder without SKSE/game initialization.
// Only the unrelated text logging service is replaced; no query or scope code is.
namespace logger
{
    inline std::size_t summaryLogCount{};
    template<class... Args> void info(const char* format, Args&&...)
    {
        if(std::string_view(format).starts_with("[Performance] smoothed ms:"))++summaryLogCount;
    }
    template<class... Args> void warn(const char*, Args&&...) {}
    template<class... Args> void error(const char*, Args&&...) {}
}
