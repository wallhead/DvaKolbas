#include "NeuralRendering/RuntimeFileLease.h"
#include <fstream>
#include <cstdio>
#include <array>
using namespace TheosRenderPipeline::NeuralRendering;
namespace {
int failed{};
void Check(bool ok, const char* name) { std::printf("%s %s\n",ok?"PASS":"FAIL",name); if(!ok)++failed; }
void Write(const std::filesystem::path& p,const char* data) { std::ofstream f(p,std::ios::binary); f<<data; if(!f) throw std::runtime_error("fixture write"); }
}
int main() {
    const auto root=std::filesystem::absolute("nr-lease-fixture-"+std::to_string(GetCurrentProcessId()));
    std::filesystem::create_directory(root);
    const auto file=root/"nvngx_dlssnr.dll", other=root/"other.dll", moved=root/"moved.dll";
    Write(file,"abc"); Write(other,"abc");
    const RuntimeProfile fixture{"fixture","NR/fixture/nvngx_dlssnr.dll",
        "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad",3};
    {
        auto lease=RuntimeFileLease::Open(file,fixture);
        Check(lease && lease->Valid() && lease->Bytes()==3 && lease->Sha256()==fixture.sha256,"KnownSha256ReadFromHeldHandle");
        if(lease) {
            Check(lease->Matches(file),"LoadedPathMatchesHeldFileId");
            Check(!lease->Matches(other),"SameBytesDifferentFileIdRejected");
            HANDLE writer=CreateFileW(file.c_str(),GENERIC_WRITE,FILE_SHARE_READ,nullptr,OPEN_EXISTING,0,nullptr);
            const auto writeError=GetLastError();
            Check(writer==INVALID_HANDLE_VALUE && writeError==ERROR_SHARING_VIOLATION,"WriteDeniedWhileVerifiedLeaseHeld");
            if(writer!=INVALID_HANDLE_VALUE) CloseHandle(writer);
            Check(!MoveFileExW(file.c_str(),moved.c_str(),MOVEFILE_REPLACE_EXISTING),"ReplacementDeniedWhileVerifiedLeaseHeld");
            auto movedLease=std::move(*lease);
            Check(movedLease.Valid() && !lease->Valid() && movedLease.Matches(file),"LeaseMoveTransfersSingleOwnership");
        }
    }
    auto wrong=fixture; wrong.bytes=4;
    auto result=RuntimeFileLease::Open(file,wrong);
    Check(!result && result.error().kind==ErrorKind::IdentityMismatch,"WrongSizeRejected");
    wrong=fixture; wrong.sha256="aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    result=RuntimeFileLease::Open(file,wrong);
    Check(!result && result.error().kind==ErrorKind::IdentityMismatch,"WrongHashRejected");
    Check(!RuntimeFileLease::Open("relative/nvngx_dlssnr.dll",fixture),"RelativeFileRejected");
    const auto missing=root/"missing.dll";
    auto absent=RuntimeFileLease::Open(missing,fixture);
    Check(!absent&&absent.error().nativeCode==ERROR_FILE_NOT_FOUND&&absent.error().message.find(missing.string())!=std::string::npos,
        "MissingArtifactErrorIdentifiesExactFileAndNativeCode");
    HANDLE writer=CreateFileW(file.c_str(),GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,nullptr,OPEN_EXISTING,0,nullptr);
    auto blocked=RuntimeFileLease::Open(file,fixture);
    Check(writer!=INVALID_HANDLE_VALUE&&!blocked&&blocked.error().nativeCode==ERROR_SHARING_VIOLATION&&blocked.error().message.find(file.string())!=std::string::npos,
        "BusyArtifactErrorIdentifiesExactFileAndNativeCode");
    if(writer!=INVALID_HANDLE_VALUE)CloseHandle(writer);
    const auto core=root/"_nvngx.dll";
    Write(core,"abc");
    auto unsignedCore=RuntimeFileLease::OpenDriverCore(core);
    Check(!unsignedCore&&unsignedCore.error().message.find("not an NVIDIA NGX module")!=std::string::npos,
        "ArbitraryFileNamedDriverCoreRejected");
    std::filesystem::remove(core);
    std::array<wchar_t,32768> system{};
    const auto count=GetSystemDirectoryW(system.data(),static_cast<UINT>(system.size()));
    if(count&&count<system.size()) {
        std::filesystem::copy_file(std::filesystem::path(system.data())/L"kernel32.dll",core);
        const auto unrelated=RuntimeFileLease::OpenDriverCore(core);
        Check(!unrelated&&unrelated.error().message.find("not an NVIDIA NGX module")!=std::string::npos,
            "RenamedTrustedWindowsModuleCannotImpersonateNgx");
        std::filesystem::remove(core);
    } else Check(false,"SystemDirectoryAvailableForSignedNonNgxFixture");
    Check(!RuntimeFileLease::OpenDriverCore("relative/_nvngx.dll"),"RelativeDriverCoreRejected");
    const auto missingCore=RuntimeFileLease::OpenDriverCore(root/"missing"/"_nvngx.dll");
    Check(!missingCore&&missingCore.error().nativeCode==ERROR_PATH_NOT_FOUND,"MissingDriverCoreKeepsNativePathError");
    Check(MoveFileExW(file.c_str(),moved.c_str(),MOVEFILE_REPLACE_EXISTING)!=FALSE,"LeaseReleasedAfterScope");
    std::filesystem::remove(moved); std::filesystem::remove(other); std::filesystem::remove(file); std::filesystem::remove(root);
    return failed?1:0;
}
