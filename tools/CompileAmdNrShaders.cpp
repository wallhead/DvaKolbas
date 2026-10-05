#include <Windows.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <filesystem>
#include <fstream>
#include <iostream>
int wmain(int argc,wchar_t** argv) {
    if(argc!=3 && argc!=4) return 2;
    std::wstring identifier=argc==4?argv[3]:L"FormatCodec";
    auto letter=[](wchar_t c){return c==L'_' || (c>=L'A' && c<=L'Z') || (c>=L'a' && c<=L'z');};
    if(identifier.empty() || !letter(identifier[0])) return 2;
    std::string symbol;
    for(auto c:identifier) {if(!letter(c) && !(c>=L'0' && c<=L'9')) return 2;symbol+=char(c);}
    Microsoft::WRL::ComPtr<ID3DBlob> code,errors;
    auto hr=D3DCompileFromFile(argv[1],nullptr,D3D_COMPILE_STANDARD_FILE_INCLUDE,"main","cs_5_1",D3DCOMPILE_ENABLE_STRICTNESS|D3DCOMPILE_IEEE_STRICTNESS|D3DCOMPILE_WARNINGS_ARE_ERRORS|D3DCOMPILE_OPTIMIZATION_LEVEL3,0,&code,&errors);
    if(errors) std::cerr.write(static_cast<const char*>(errors->GetBufferPointer()),errors->GetBufferSize());
    if(FAILED(hr) || !code || !code->GetBufferSize()) return 2;
    auto temporary=std::filesystem::path(argv[2]);temporary+=L".tmp";
    std::ofstream file(temporary,std::ios::binary|std::ios::trunc);
    file<<"#pragma once\n#include <cstdint>\nnamespace AmdNrShader { inline constexpr std::uint8_t "<<symbol<<"[]{\n";
    auto bytes=static_cast<const unsigned char*>(code->GetBufferPointer());
    for(std::size_t i=0;i<code->GetBufferSize();++i) {file<<unsigned(bytes[i])<<',';if(i%32==31) file<<'\n';}
    file<<"\n}; }\n";file.close();
    if(!file || !MoveFileExW(temporary.c_str(),argv[2],MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)) return 2;
    return 0;
}
