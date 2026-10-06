#include "DriverCoreTrust.h"
#include <bcrypt.h>
#include <wincrypt.h>
#include <wintrust.h>
#include <softpub.h>
#include <mscat.h>
#include <array>
#include <vector>
#include <string>

namespace TheosRenderPipeline::NeuralRendering {
namespace {
bool IsNgxMetadata(const std::filesystem::path& path) {
    // Read the candidate's embedded resource, never metadata merged from an
    // external MUI sidecar. These flags map resources without executing code.
    struct ResourceModule {
        HMODULE module{};
        ~ResourceModule(){if(module)FreeLibrary(module);}
    } image{LoadLibraryExW(path.c_str(),nullptr,LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE|LOAD_LIBRARY_AS_IMAGE_RESOURCE)};
    if(!image.module)return false;
    const auto resource=FindResourceW(image.module,MAKEINTRESOURCEW(VS_VERSION_INFO),MAKEINTRESOURCEW(16)); // RT_VERSION
    const auto size=resource?SizeofResource(image.module,resource):0;
    const auto loaded=resource?LoadResource(image.module,resource):nullptr;
    const auto bytes=loaded?LockResource(loaded):nullptr;
    if(!bytes || !size || size>1024*1024)return false;
    std::vector<BYTE> data(static_cast<const BYTE*>(bytes),static_cast<const BYTE*>(bytes)+size);
    struct Translation {WORD language,codepage;};
    Translation* translations{};UINT length{};
    if(!VerQueryValueW(data.data(),L"\\VarFileInfo\\Translation",reinterpret_cast<void**>(&translations),&length))return false;
    for(UINT i=0;i<length/sizeof(Translation);++i) {
        const auto value=[&](const wchar_t* key,const wchar_t* expected) {
            std::array<wchar_t,128> query{};
            swprintf_s(query.data(),query.size(),L"\\StringFileInfo\\%04x%04x\\%s",translations[i].language,translations[i].codepage,key);
            wchar_t* text{};UINT chars{};
            return VerQueryValueW(data.data(),query.data(),reinterpret_cast<void**>(&text),&chars) &&
                text && chars && text[chars-1]==L'\0' && _wcsicmp(text,expected)==0;
        };
        if(value(L"CompanyName",L"NVIDIA Corporation") && value(L"OriginalFilename",L"nvngx.dll"))return true;
    }
    return false;
}

bool TrustedPublisher(WINTRUST_DATA& data) {
    auto* provider=WTHelperProvDataFromStateData(data.hWVTStateData);
    auto* signer=provider?WTHelperGetProvSignerFromChain(provider,0,FALSE,0):nullptr;
    if(!signer || !signer->csCertChain || !signer->pasCertChain || !signer->pasCertChain[0].pCert)return false;
    std::array<wchar_t,256> name{};
    const auto size=CertGetNameStringW(signer->pasCertChain[0].pCert,CERT_NAME_SIMPLE_DISPLAY_TYPE,0,nullptr,name.data(),static_cast<DWORD>(name.size()));
    if(size<=1 || size>name.size())return false;
    return _wcsicmp(name.data(),L"NVIDIA Corporation")==0 ||
        _wcsicmp(name.data(),L"Microsoft Windows Hardware Compatibility Publisher")==0;
}

bool Verify(WINTRUST_DATA& data) {
    GUID action=WINTRUST_ACTION_GENERIC_VERIFY_V2;
    data.cbStruct=sizeof(data);data.dwUIChoice=WTD_UI_NONE;
    data.fdwRevocationChecks=WTD_REVOKE_NONE;
    data.dwProvFlags=WTD_CACHE_ONLY_URL_RETRIEVAL|WTD_REVOCATION_CHECK_NONE;
    data.dwStateAction=WTD_STATEACTION_VERIFY;
    const auto window=reinterpret_cast<HWND>(INVALID_HANDLE_VALUE);
    const auto status=WinVerifyTrust(window,&action,&data);
    const bool trusted=status==ERROR_SUCCESS && TrustedPublisher(data);
    data.dwStateAction=WTD_STATEACTION_CLOSE;
    WinVerifyTrust(window,&action,&data);
    return trusted;
}

bool CatalogTrust(const std::filesystem::path& path,HANDLE file,const wchar_t* algorithm) {
    struct Catalogs {
        HCATADMIN admin{};HCATINFO catalog{};
        ~Catalogs(){if(catalog)CryptCATAdminReleaseCatalogContext(admin,catalog,0);if(admin)CryptCATAdminReleaseContext(admin,0);}
    } catalogs;
    if(!CryptCATAdminAcquireContext2(&catalogs.admin,nullptr,algorithm,nullptr,0))return false;
    DWORD size{};
    if(!CryptCATAdminCalcHashFromFileHandle2(catalogs.admin,file,&size,nullptr,0) || !size || size>128)return false;
    std::vector<BYTE> hash(size);
    if(!CryptCATAdminCalcHashFromFileHandle2(catalogs.admin,file,&size,hash.data(),0))return false;
    std::wstring tag;constexpr wchar_t hex[]=L"0123456789ABCDEF";
    for(DWORD i=0;i<size;++i){tag+=hex[hash[i]>>4];tag+=hex[hash[i]&15];}
    // Enumeration releases the previous catalog when requesting the next one.
    for(unsigned count=0;count<32;++count) {
        catalogs.catalog=CryptCATAdminEnumCatalogFromHash(catalogs.admin,hash.data(),size,0,catalogs.catalog?&catalogs.catalog:nullptr);
        if(!catalogs.catalog)break;
        CATALOG_INFO info{};info.cbStruct=sizeof(info);
        if(!CryptCATCatalogInfoFromContext(catalogs.catalog,&info,0))continue;
        WINTRUST_CATALOG_INFO member{};member.cbStruct=sizeof(member);
        member.pcwszCatalogFilePath=info.wszCatalogFile;member.pcwszMemberTag=tag.c_str();
        member.pcwszMemberFilePath=path.c_str();member.hMemberFile=file;
        member.pbCalculatedFileHash=hash.data();member.cbCalculatedFileHash=size;member.hCatAdmin=catalogs.admin;
        WINTRUST_DATA data{};data.dwUnionChoice=WTD_CHOICE_CATALOG;data.pCatalog=&member;
        if(Verify(data))return true;
    }
    return false;
}
}

Result<void> VerifyDriverCoreTrust(const std::filesystem::path& path,HANDLE file) {
    if(!IsNgxMetadata(path))return std::unexpected(Error{ErrorKind::IdentityMismatch,0,"Driver core is not an NVIDIA NGX module (CompanyName/OriginalFilename)"});
    WINTRUST_FILE_INFO info{};info.cbStruct=sizeof(info);info.pcwszFilePath=path.c_str();info.hFile=file;
    WINTRUST_DATA data{};data.dwUnionChoice=WTD_CHOICE_FILE;data.pFile=&info;
    if(Verify(data) || CatalogTrust(path,file,BCRYPT_SHA256_ALGORITHM) || CatalogTrust(path,file,BCRYPT_SHA1_ALGORITHM))return {};
    return std::unexpected(Error{ErrorKind::IdentityMismatch,0,"Driver core has no trusted NVIDIA/Windows hardware signature or installed catalog; verification is offline"});
}
}
