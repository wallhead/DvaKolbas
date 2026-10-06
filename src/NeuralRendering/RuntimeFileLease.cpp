#include "RuntimeFileLease.h"
#include "DriverCoreTrust.h"
#include <bcrypt.h>
#include <array>
#include <algorithm>
#include <vector>
#include <utility>
namespace TheosRenderPipeline::NeuralRendering {
namespace {
std::unexpected<Error> Fail(ErrorKind kind, const char* message, int64_t native=0) {
    return std::unexpected(Error{kind,native,message});
}
Result<std::string> HashHandle(HANDLE file) {
    struct Hash {
        BCRYPT_ALG_HANDLE algorithm{}; BCRYPT_HASH_HANDLE value{};
        // Members remain alive during the destructor body: CNG must destroy
        // the handle before releasing its caller-owned backing storage.
        std::vector<unsigned char> object;
        ~Hash(){ if(value) BCryptDestroyHash(value); if(algorithm) BCryptCloseAlgorithmProvider(algorithm,0); }
    } hash;
    auto code=BCryptOpenAlgorithmProvider(&hash.algorithm,BCRYPT_SHA256_ALGORITHM,nullptr,0);
    if(code<0) return Fail(ErrorKind::Io,"Cannot open SHA256 algorithm",code);
    DWORD size{},written{};
    code=BCryptGetProperty(hash.algorithm,BCRYPT_OBJECT_LENGTH,reinterpret_cast<PUCHAR>(&size),sizeof(size),&written,0);
    if(code<0 || written!=sizeof(size)) return Fail(ErrorKind::Io,"Cannot get SHA256 object size",code);
    hash.object.resize(size);
    code=BCryptCreateHash(hash.algorithm,&hash.value,hash.object.data(),size,nullptr,0,0);
    if(code<0) return Fail(ErrorKind::Io,"Cannot create SHA256 hash",code);
    std::array<unsigned char,65536> chunk{};
    for(;;) {
        DWORD count{};
        if(!ReadFile(file,chunk.data(),static_cast<DWORD>(chunk.size()),&count,nullptr))
            return Fail(ErrorKind::Io,"Cannot read locked NR file",GetLastError());
        if(!count) break;
        code=BCryptHashData(hash.value,chunk.data(),count,0);
        if(code<0) return Fail(ErrorKind::Io,"Cannot hash locked NR file",code);
    }
    std::array<unsigned char,32> digest{};
    code=BCryptFinishHash(hash.value,digest.data(),static_cast<ULONG>(digest.size()),0);
    if(code<0) return Fail(ErrorKind::Io,"Cannot finish locked NR hash",code);
    std::string hex; hex.reserve(64);
    constexpr char digits[]="0123456789abcdef";
    for(auto byte:digest) {hex+=digits[byte>>4];hex+=digits[byte&15];}
    return hex;
}
}
RuntimeFileLease::~RuntimeFileLease() { if (Valid()) CloseHandle(file_); }
RuntimeFileLease::RuntimeFileLease(RuntimeFileLease&& other) noexcept { *this = std::move(other); }
RuntimeFileLease& RuntimeFileLease::operator=(RuntimeFileLease&& other) noexcept {
    if (this!=&other) {
        if (Valid()) CloseHandle(file_);
        file_=std::exchange(other.file_,INVALID_HANDLE_VALUE);
        path_=std::move(other.path_); sha256_=std::move(other.sha256_); bytes_=other.bytes_;
    }
    return *this;
}
Result<RuntimeFileLease> RuntimeFileLease::Open(const std::filesystem::path& path, const RuntimeProfile& profile) {
    if(profile.sha256.size()!=64 ||
        !std::ranges::all_of(profile.sha256,[](char c){return (c>='0'&&c<='9')||(c>='a'&&c<='f');}))
        return Fail(ErrorKind::InvalidInput,"NR file path/hash is invalid");
    auto lease=OpenHeld(path,profile.id);
    if(!lease)return std::unexpected(lease.error());
    const auto mismatch=[&](std::string message) {
        return std::unexpected(Error{ErrorKind::IdentityMismatch,0,"NR artifact ["+std::string(profile.id)+"] path="+path.string()+": "+message+"; native=0"});
    };
    if(lease->bytes_!=profile.bytes)
        return mismatch("Held size differs from qualified profile; observed="+std::to_string(lease->bytes_)+" expected="+std::to_string(profile.bytes));
    if(lease->sha256_!=profile.sha256)
        return mismatch("Held SHA256 differs from qualified profile; observed="+lease->sha256_+" expected="+std::string(profile.sha256));
    return lease;
}
Result<RuntimeFileLease> RuntimeFileLease::OpenDriverCore(const std::filesystem::path& path) {
    if(_wcsicmp(path.filename().c_str(),L"_nvngx.dll")!=0 && _wcsicmp(path.filename().c_str(),L"nvngx.dll")!=0)
        return Fail(ErrorKind::InvalidInput,"NR driver-core filename must be _nvngx.dll or nvngx.dll");
    auto lease=OpenHeld(path,"driver-core");
    if(!lease)return std::unexpected(lease.error());
    auto trusted=VerifyDriverCoreTrust(path,lease->file_);
    if(!trusted)return std::unexpected(Error{trusted.error().kind,trusted.error().nativeCode,
        "NR artifact [driver-core] path="+path.string()+": "+trusted.error().message+"; sha256="+lease->sha256_});
    return lease;
}
Result<RuntimeFileLease> RuntimeFileLease::OpenHeld(const std::filesystem::path& path, std::string_view artifact) {
    const auto fileFailure=[&](ErrorKind kind,std::string_view message,int64_t native=0) {
        return std::unexpected(Error{kind,native,"NR artifact ["+std::string(artifact)+"] path="+path.string()+": "+
            std::string(message)+"; native="+std::to_string(native)});
    };
    if(!path.is_absolute())return fileFailure(ErrorKind::InvalidInput,"NR file path must be absolute");
    RuntimeFileLease lease;
    lease.file_=CreateFileW(path.c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OPEN_REPARSE_POINT|FILE_FLAG_SEQUENTIAL_SCAN,nullptr);
    if(!lease.Valid()) return fileFailure(ErrorKind::Io,"Cannot open/lock file against write/delete",GetLastError());
    FILE_ATTRIBUTE_TAG_INFO attributes{};
    if(!GetFileInformationByHandleEx(lease.file_,FileAttributeTagInfo,&attributes,sizeof(attributes)))
        return fileFailure(ErrorKind::Io,"Cannot inspect held file",GetLastError());
    if(attributes.FileAttributes&(FILE_ATTRIBUTE_REPARSE_POINT|FILE_ATTRIBUTE_DIRECTORY))
        return fileFailure(ErrorKind::InvalidInput,"Artifact is a reparse point or directory");
    LARGE_INTEGER bytes{};
    if(!GetFileSizeEx(lease.file_,&bytes)) return fileFailure(ErrorKind::Io,"Cannot read held size",GetLastError());
    if(bytes.QuadPart<0)return fileFailure(ErrorKind::IdentityMismatch,"Held file size is invalid");
    lease.bytes_=static_cast<uint64_t>(bytes.QuadPart);
    auto digest=HashHandle(lease.file_);
    if(!digest) return fileFailure(digest.error().kind,digest.error().message,digest.error().nativeCode);
    lease.sha256_=std::move(*digest);
    // Keep the absolute path used for LoadLibraryEx. Mapping identity is checked
    // again from the loaded module path against this held handle, not by strings.
    lease.path_=path;
    return lease;
}
bool RuntimeFileLease::Matches(const std::filesystem::path& path) const noexcept {
    if(!Valid() || !path.is_absolute()) return false;
    const auto opened=CreateFileW(path.c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,nullptr);
    if(opened==INVALID_HANDLE_VALUE) return false;
    FILE_ID_INFO a{},b{};
    const bool match=GetFileInformationByHandleEx(file_,FileIdInfo,&a,sizeof(a)) &&
        GetFileInformationByHandleEx(opened,FileIdInfo,&b,sizeof(b)) && a.VolumeSerialNumber==b.VolumeSerialNumber &&
        std::equal(std::begin(a.FileId.Identifier),std::end(a.FileId.Identifier),std::begin(b.FileId.Identifier));
    CloseHandle(opened);
    return match;
}
}
