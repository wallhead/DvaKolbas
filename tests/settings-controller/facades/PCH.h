#pragma once
// Only the game's PCH/logging is replaced; settings/controller code is unchanged.
#include <Windows.h>
#include <algorithm>
#include <atomic>
#include <filesystem>
#include <format>
#include <iostream>
#include <string>
namespace logger {
template<class... A> void info(std::format_string<A...> f,A&&... a){std::cerr<<std::format(f,std::forward<A>(a)...)<<'\n';}
template<class... A> void error(std::format_string<A...> f,A&&... a){info(f,std::forward<A>(a)...);}
}
