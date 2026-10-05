#include "ModelIdentity.h"
#include <Windows.h>
#include <bcrypt.h>
#include <algorithm>
#include <limits>
#include <memory>
#include <new>
namespace TheosRenderPipeline::NeuralRendering::Amd {
std::expected<Sha256Digest,IdentityError> Sha256(std::span<const std::byte> bytes) {
    struct Algorithm { BCRYPT_ALG_HANDLE h{}; ~Algorithm(){if(h) BCryptCloseAlgorithmProvider(h,0);} } algorithm;
    struct Hash { BCRYPT_HASH_HANDLE h{}; ~Hash(){if(h) BCryptDestroyHash(h);} };
    auto fail=[](){return std::unexpected(IdentityError::CryptoProvider);};
    if(BCryptOpenAlgorithmProvider(&algorithm.h,BCRYPT_SHA256_ALGORITHM,nullptr,0)<0) return fail();
    try {
        ULONG objectSize{},resultSize{},digestSize{};
        if(BCryptGetProperty(algorithm.h,BCRYPT_OBJECT_LENGTH,reinterpret_cast<PUCHAR>(&objectSize),sizeof(objectSize),&resultSize,0)<0 || resultSize!=sizeof(objectSize)) return fail();
        if(BCryptGetProperty(algorithm.h,BCRYPT_HASH_LENGTH,reinterpret_cast<PUCHAR>(&digestSize),sizeof(digestSize),&resultSize,0)<0 || digestSize!=32) return fail();
        // Backing memory outlives the hash handle (reverse destruction order).
        std::vector<unsigned char> object(objectSize);Hash hash;
        if(BCryptCreateHash(algorithm.h,&hash.h,object.data(),objectSize,nullptr,0,0)<0) return fail();
        while(!bytes.empty()) {
            auto count=ULONG(std::min(bytes.size(),std::size_t(std::numeric_limits<ULONG>::max())));
            if(BCryptHashData(hash.h,reinterpret_cast<PUCHAR>(const_cast<std::byte*>(bytes.data())),count,0)<0) return fail();
            bytes=bytes.subspan(count);
        }
        Sha256Digest digest{};
        if(BCryptFinishHash(hash.h,reinterpret_cast<PUCHAR>(digest.data()),ULONG(digest.size()),0)<0) return fail();
        return digest;
    } catch(const std::bad_alloc&) {return std::unexpected(IdentityError::Memory);}
}
std::expected<ModelIdentification,IdentityError> IdentifyModel(const ArchiveFile& file) {
    const struct {const char* name;std::uint64_t size;} known[]{
#include "KnownModelRecords.inc"
    };
    auto digest=Sha256(file.Bytes());if(!digest) return std::unexpected(digest.error());
    constexpr char hex[]="6bf8dc931ef3ccffe18c82de26ab374156e7f19539ffcf8eabaa25dca5cf15ab";
    bool match=file.Index().fileSize==147689451 && file.Index().payloadStart==5673 && file.Index().records.size()==std::size(known);
    if(match) for(std::size_t i=0;i<std::size(known);++i) match &= file.Index().records[i].name==known[i].name && file.Index().records[i].size==known[i].size;
    const auto nibble=[](char c){return c<='9'?c-'0':c-'a'+10;};
    for(unsigned i=0;i<32;++i) match &= std::to_integer<unsigned>((*digest)[i])==unsigned(nibble(hex[2*i])*16+nibble(hex[2*i+1]));
    try {return ModelIdentification{match?ModelStatus::KnownArchiveIncompleteSchema:ModelStatus::UnsupportedArchive,*digest,
        match?"Exact archive identified; tensor views and inference contracts remain incomplete.":"No supported archive identity matches this snapshot."};}
    catch(const std::bad_alloc&) {return std::unexpected(IdentityError::Memory);}
}
}
