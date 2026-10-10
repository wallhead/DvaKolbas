#pragma once
#include <cstdint>
struct XellFixtureCall { std::uint32_t kind{},id{},marker{},enabled{},minimumIntervalUs{}; };
// Test-only exports, not in the public runtime table or production package.
using XellFixtureReset=void(*)();
using XellFixtureCount=std::uint32_t(*)();
using XellFixtureRead=XellFixtureCall(*)(std::uint32_t);
using XellFixtureFailNext=void(*)(std::int32_t);
