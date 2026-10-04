#pragma once
// Standalone production DLSS backend: replace only game text logging/version.
#include <Windows.h>
#include <algorithm>
#include <climits>
#include <cmath>
#include <filesystem>
#include <format>
#include <iostream>
#include <string_view>
namespace Plugin { inline constexpr std::string_view VERSION_STRING="0.3.5"; }
namespace logger {
template<class... A> void info(std::format_string<A...> text,A&&... args){std::cerr<<std::format(text,std::forward<A>(args)...)<<'\n';}
template<class... A> void warn(std::format_string<A...> text,A&&... args){info(text,std::forward<A>(args)...);}
template<class... A> void error(std::format_string<A...> text,A&&... args){info(text,std::forward<A>(args)...);}
}
