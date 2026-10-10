#pragma once
#include <cstdint>
struct XellFixtureCall { std::uint32_t kind{},id{},marker{},enabled{},minimumIntervalUs{}; };
// Test-only exports, not in the public runtime table or production package.
using XellFixtureReset=void(*)();
using XellFixtureCount=std::uint32_t(*)();
using XellFixtureRead=XellFixtureCall(*)(std::uint32_t);
using XellFixtureFailNext=void(*)(std::int32_t);
struct XessFgFixtureCall { std::uint32_t kind{},id{},value{}; };
using XessFgFixtureRead=XessFgFixtureCall(*)(std::uint32_t);
using XessFgFixtureFailAt=void(*)(std::uint32_t);
using XessFgFixtureObserver=void(*)(std::uint32_t);
using XessFgFixtureObserve=void(*)(XessFgFixtureObserver);
