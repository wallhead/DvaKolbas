#pragma once
#include "NeuralRendering/Amd/NumericFormats.h"
#include <Windows.h>
#include <atomic>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string_view>
#include <vector>
namespace AmdNrTools {
inline auto ParseFormat(std::wstring_view value) {
    using namespace TheosRenderPipeline::NeuralRendering::Amd;
    if(value==L"f32-to-f16") return FormatOperation::Float32ToHalf;
    if(value==L"f16-to-f32") return FormatOperation::HalfToFloat32;
    if(value==L"e4m3-to-f16") return FormatOperation::E4m3ToHalf;
    if(value==L"f32-to-e4m3") return FormatOperation::Float32ToE4m3;
    throw std::runtime_error("invalid format");
}
inline void CheckDistinct(const std::filesystem::path& input,const std::filesystem::path& output) {
    std::error_code error;
    bool equivalent=std::filesystem::equivalent(input,output,error);
    if(equivalent || std::filesystem::weakly_canonical(input)==std::filesystem::weakly_canonical(output)) throw std::runtime_error("input and output alias");
}
inline std::vector<std::uint32_t> ReadWords(const std::filesystem::path& path) {
    std::ifstream file(path,std::ios::binary|std::ios::ate);
    if(!file) throw std::runtime_error("input cannot be opened");
    const auto size=file.tellg();
    if(size<0 || size>268435456 || size%4) throw std::runtime_error("input size must be a multiple of four and at most 256 MiB");
    std::vector<std::uint32_t> words(std::size_t(size)/4);file.seekg(0);
    if(size && !file.read(reinterpret_cast<char*>(words.data()),size)) throw std::runtime_error("incomplete input read");
    if(file.peek()!=std::char_traits<char>::eof()) throw std::runtime_error("input changed during read");
    return words;
}
inline void PublishWords(const std::filesystem::path& output,std::span<const std::uint32_t> words) {
    static std::atomic<unsigned long long> counter{};
    struct Temporary {
        std::filesystem::path path;HANDLE handle{INVALID_HANDLE_VALUE};bool owned{},published{};
        ~Temporary(){if(handle!=INVALID_HANDLE_VALUE) CloseHandle(handle);if(owned && !published){std::error_code e;std::filesystem::remove(path,e);}}
    } temporary;
    for(unsigned i=0;i<100;++i) {
        temporary.path=output;temporary.path+=L".tmp-"+std::to_wstring(GetCurrentProcessId())+L"-"+std::to_wstring(counter++);
        temporary.handle=CreateFileW(temporary.path.c_str(),GENERIC_WRITE,0,nullptr,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,nullptr);
        if(temporary.handle!=INVALID_HANDLE_VALUE) {temporary.owned=true;break;}
        if(GetLastError()!=ERROR_FILE_EXISTS && GetLastError()!=ERROR_ALREADY_EXISTS) throw std::runtime_error("temporary output cannot be created");
        // Never remove a colliding file owned by someone else.
        temporary.path.clear();
    }
    if(temporary.handle==INVALID_HANDLE_VALUE) throw std::runtime_error("temporary output collisions");
    const auto bytes=std::as_bytes(words);std::size_t offset=0;
    while(offset<bytes.size()) {
        DWORD written{},count=DWORD(std::min(bytes.size()-offset,std::size_t{1048576}));
        if(!WriteFile(temporary.handle,bytes.data()+offset,count,&written,nullptr) || written!=count) throw std::runtime_error("incomplete output write");
        offset+=written;
    }
    if(!FlushFileBuffers(temporary.handle)) throw std::runtime_error("output flush failed");
    if(!CloseHandle(temporary.handle)) throw std::runtime_error("output close failed");
    temporary.handle=INVALID_HANDLE_VALUE;
    if(!MoveFileExW(temporary.path.c_str(),output.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)) throw std::runtime_error("output publication failed");
    temporary.published=true;
}
}
