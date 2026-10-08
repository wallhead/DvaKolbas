#pragma once
#include <string_view>

namespace TheosRenderPipeline
{
// NVAPI and DXGI identify products with the same whole-word RTX test.
// TITAN RTX ends at the token; GTX 16 and RTX-like substrings are excluded.
template<class Char> constexpr bool IsRtxProductName(std::basic_string_view<Char> name)
{
    const auto separator = [](Char c) {
        return c == Char(' ') || c == Char('\t') || c == Char('\r') || c == Char('\n') ||
            c == Char('\f') || c == Char('\v');
    };
    for (std::size_t i = 0; i + 3 <= name.size(); ++i) {
        if (name[i] == Char('R') && name[i + 1] == Char('T') && name[i + 2] == Char('X') &&
            (i == 0 || separator(name[i - 1])) && (i + 3 == name.size() || separator(name[i + 3])))
            return true;
    }
    return false;
}
}
