#pragma once
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <span>
#include <string>
#include <vector>

namespace AmdNrTest {
inline void Require(bool ok, const char* why) {
    if (!ok) { std::fprintf(stderr, "FAIL: %s\n", why); std::exit(1); }
}
inline void Put(std::vector<std::byte>& bytes, std::size_t pos, std::uint64_t value, unsigned width) {
    Require(pos <= bytes.size() && width <= bytes.size()-pos, "fixture write range");
    for (unsigned i=0;i<width;++i) bytes[pos+i]=std::byte((value>>(8*i))&255);
}
struct TestRecord { std::string name; std::vector<std::byte> payload; };
inline std::vector<std::byte> MakeArchive(std::span<const TestRecord> records) {
    std::size_t base=16;
    for (const auto& r:records) base+=1+r.name.size()+16;
    std::vector<std::byte> bytes(base);
    const char magic[]="DLSSNRW1";
    for (unsigned i=0;i<8;++i) bytes[i]=std::byte(magic[i]);
    Put(bytes,8,records.size(),4); Put(bytes,12,base,4);
    std::size_t p=16,offset=0;
    for (const auto& r:records) {
        Put(bytes,p++,r.name.size(),1);
        for (char c:r.name) bytes[p++]=std::byte(c);
        Put(bytes,p,offset,8); p+=8; Put(bytes,p,r.payload.size(),8);p+=8;
        offset+=r.payload.size();
    }
    for (const auto& r:records) bytes.insert(bytes.end(),r.payload.begin(),r.payload.end());
    return bytes;
}
inline std::vector<std::byte> Fixture() {
    const TestRecord records[]{{"first",{std::byte{1},std::byte{2},std::byte{3}}},
                               {"second",{std::byte{4},std::byte{5}}}};
    return MakeArchive(records);
}
inline void Write(const std::filesystem::path& path,std::span<const std::byte> data) {
    std::ofstream f(path,std::ios::binary);f.write(reinterpret_cast<const char*>(data.data()),data.size());
    Require(bool(f),"fixture file write");
}
}
