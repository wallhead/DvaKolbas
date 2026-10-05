#include "NeuralRendering/Amd/C512ProjectionReference.h"
#include "NeuralRendering/Amd/C512ProjectionProbe.h"
#include "NeuralRendering/Amd/NumericFormats.h"
#include "TestSupport.h"
#include <array>
#include <algorithm>
#include "../../tools/AmdNrDevices.h"
#include <d3d12sdklayers.h>
#include <filesystem>
#include <fstream>
using namespace TheosRenderPipeline::NeuralRendering::Amd;
using AmdNrTest::Require;

namespace {
std::size_t WeightAddress(unsigned n,unsigned k) {
    const auto lane=16*((k%16)/8)+n%16;
    auto v2=lane>>1;
    const auto v25=v2&8,v31=(lane<<2)&8;
    v2=(v2&6)|(lane&1);
    return ((v2<<6)|(v25<<2))+v31+(n/16)*512+(k/32)*0x4000+
        (((k%16)/4)%2)*16+((k/16)%2)*4+k%4;
}
C512ProjectionWeights Weights(std::span<const std::uint8_t> matrix,const std::array<std::uint16_t,512>& coefficients={}) {
    std::vector<std::byte> raw(263168);
    for(unsigned n=0;n<512;++n) for(unsigned k=0;k<512;++k) raw[WeightAddress(n,k)]=std::byte(matrix[n*512+k]);
    for(unsigned thread=0;thread<256;++thread) for(unsigned group=thread>>5;group<32;group+=8) {
        const auto n=16*group|(thread&15);
        const auto address=262144+(((n&0x1f1)|((thread<<2)&8))<<1)|(thread&12);
        AmdNrTest::Put(raw,address,coefficients[n],2);
    }
    return DecodeC512Projection(raw).value();
}
std::vector<std::uint32_t> Evaluate(const C512TensorLayout& l,std::span<const std::uint8_t> input,
    std::span<const std::uint8_t> residual,const C512ProjectionWeights& weights) {
    std::vector<std::uint32_t> out(l.ByteCount()*C512ProjectionWordsPerValue,0xa5a5a5a5);
    Require(bool(EvaluateC512ProjectionReference(l,input,residual,weights,out)),"CPU ordered reference");
    for(std::size_t i=0;i<out.size();i+=2) Require(!(out[i]&0xff000000) && !(out[i+1]&0xffff0000),"diagnostic zero extension");
    return out;
}
void AsymmetricPixelChannels() {
    const auto l=MakeC512TensorLayout({4,4}).value();
    std::vector<std::uint8_t> matrix(262144),input(l.ByteCount());
    for(unsigned k=0;k<512;++k) matrix[((37*k+11)%512)*512+k]=0x38;
    for(unsigned p=0;p<16;++p) for(unsigned k=0;k<512;++k) input[p*512+k]=std::uint8_t((p+k)%126+1|(((p+k)&1)<<7));
    const auto weights=Weights(matrix);
    const auto out=Evaluate(l,input,{},weights);
    for(unsigned p=0;p<16;++p) for(unsigned k=0;k<512;++k) {
        const auto at=2*(p*512+(37*k+11)%512);
        const auto code=input[p*512+k];
        Require(out[at]==(std::uint32_t(code)<<16|DecodeE4m3ToHalf(code)) && out[at+1]==0,"asymmetric pixel/channel values");
    }
}
void ResidualAndChunkBoundaries() {
    const auto l=MakeC512TensorLayout({4,4}).value();
    std::vector<std::uint8_t> matrix(262144),input(l.ByteCount(),1),residual(l.ByteCount(),0xb0);
    std::array<std::uint16_t,512> coef{};coef.fill(0x3800);
    auto w=Weights(matrix,coef);
    auto out=Evaluate(l,input,residual,w);
    for(std::size_t i=0;i<out.size();i+=2) Require(out[i]==0x00a8b400 && out[i+1]==0xb400,"zero matrix preserves early half residual");
}
void ChunkRounding() {
    const auto l=MakeC512TensorLayout({4,4}).value();
    std::vector<std::uint8_t> matrix(262144),input(l.ByteCount(),1);
    std::fill(matrix.begin(),matrix.begin()+512,1);matrix[0]=0x7e;
    const auto weights=Weights(matrix);const auto out=Evaluate(l,input,{},weights);
    for(unsigned p=0;p<16;++p) Require(out[p*1024]==0x00363b00 && out[p*1024+1]==0,"half boundary per 32 terms");
}
void EarlyResidualRounding() {
    const auto l=MakeC512TensorLayout({4,4}).value();
    std::vector<std::uint8_t> matrix(262144),input(l.ByteCount()),residual(l.ByteCount());
    matrix[0]=0xb8;
    std::array<std::uint16_t,512> coef{};coef[0]=0x3c01;
    for(unsigned p=0;p<16;++p) { input[p*512]=0x39;residual[p*512]=0x39; }
    const auto weights=Weights(matrix,coef);const auto out=Evaluate(l,input,residual,weights);
    for(unsigned p=0;p<16;++p) Require(out[p*1024]==0x00001400 && out[p*1024+1]==0x3c81,"early residual half rounding");
}
void SpecialValues() {
    const auto l=MakeC512TensorLayout({4,4}).value();
    std::vector<std::uint8_t> matrix(262144),input(l.ByteCount()),residual(l.ByteCount(),0x80);
    matrix[511*512]=0x7f;
    std::array<std::uint16_t,512> coef{};coef.fill(0x3c00);
    coef[0]=coef[1]=coef[2]=0x7c00;coef[3]=0x7d01;
    for(unsigned p=0;p<16;++p) {
        residual[p*512]=0;residual[p*512+1]=0x38;residual[p*512+2]=0xb8;residual[p*512+3]=0x38;
    }
    const auto weights=Weights(matrix,coef);const auto out=Evaluate(l,input,residual,weights);
    constexpr std::uint32_t final[]{0x007f7e00,0x007e7c00,0x00fefc00,0x007f7e00};
    constexpr std::uint16_t initial[]{0x7e00,0x7c00,0xfc00,0x7e00};
    for(unsigned p=0;p<16;++p) for(unsigned n=0;n<512;++n) {
        const auto at=2*(p*512+n);
        Require(out[at]==(n<4?final[n]:n==511?0x007f7e00:0),"special final half/encoding");
        Require(out[at+1]==(n<4?initial[n]:0),"special and negative-zero initialization");
    }
}
void ValidationBeforeWrites() {
    const auto l=MakeC512TensorLayout({4,4}).value();
    std::vector<std::uint8_t> matrix(262144),input(l.ByteCount()),residual(l.ByteCount());
    auto weights=Weights(matrix);
    std::vector<std::uint32_t> output(l.ByteCount()*2,0x69426942);
    auto check=[&](const auto& layout,std::span<const std::uint8_t> in,std::span<const std::uint8_t> res,
                  const auto& w,std::span<std::uint32_t> out,ProjectionEvaluationError error) {
        const auto old=output;
        const auto r=EvaluateC512ProjectionReference(layout,in,res,w,out);
        Require(!r && r.error()==error && output==old,"validation before writes");
    };
    check(l,std::span(input).first(input.size()-1),{},weights,output,ProjectionEvaluationError::Count);
    check(l,input,std::span(residual).first(residual.size()-1),weights,output,ProjectionEvaluationError::Count);
    check(l,input,{},weights,std::span(output).first(output.size()-1),ProjectionEvaluationError::Count);
    check(MakeC512TensorLayout({20,16}).value(),input,{},weights,output,ProjectionEvaluationError::Extent);
    auto bytes=std::span(reinterpret_cast<const std::uint8_t*>(output.data()),l.ByteCount());
    check(l,bytes,{},weights,output,ProjectionEvaluationError::Overlap);
    check(l,input,bytes,weights,output,ProjectionEvaluationError::Overlap);
    auto owner=std::move(weights);
    check(l,input,{},weights,output,ProjectionEvaluationError::Weights);
    Require(bool(ValidateC512ProjectionInputs(l,input,{},owner)),"moved-to weights valid");
}
}
struct GpuContext {
    AmdNrTools::ComPtr<ID3D12Device> device;
    AmdNrTools::ComPtr<ID3D12CommandQueue> queue;
    AmdNrTools::ComPtr<ID3D12InfoQueue> info;
    GpuContext(bool warp,LUID luid={},bool debug=false) {
        auto adapter=AmdNrTools::SelectAdapter(warp,luid);AmdNrTools::Describe(adapter.Get());
        AmdNrTools::CheckDeviceApi(D3D12CreateDevice(adapter.Get(),D3D_FEATURE_LEVEL_11_0,IID_PPV_ARGS(&device)));
        D3D12_COMMAND_QUEUE_DESC desc{};
        AmdNrTools::CheckDeviceApi(device->CreateCommandQueue(&desc,IID_PPV_ARGS(&queue)));
        if(debug) AmdNrTools::CheckDeviceApi(device.As(&info));
    }
    void CheckMessages() {
        if(info) for(UINT64 i=0;i<info->GetNumStoredMessagesAllowedByRetrievalFilter();++i) {
            SIZE_T size{};info->GetMessage(i,nullptr,&size);std::vector<std::byte> storage(size);
            auto message=reinterpret_cast<D3D12_MESSAGE*>(storage.data());
            AmdNrTools::CheckDeviceApi(info->GetMessage(i,message,&size));
            if(message->Severity<=D3D12_MESSAGE_SEVERITY_WARNING) {
                std::fprintf(stderr,"D3D12: %s\n",message->pDescription);Require(false,"validation warning/error");
            }
        }
    }
};
void GpuTests(bool warp,LUID luid,bool debug) {
    GpuContext context(warp,luid,debug);
    auto probe=C512ProjectionProbe::Create(context.device.Get(),context.queue.Get());Require(bool(probe),"projection probe");
    Require(!C512ProjectionProbe::Create(nullptr,context.queue.Get()),"null device");
    Require(!C512ProjectionProbe::Create(context.device.Get(),nullptr),"null queue");
    D3D12_COMMAND_QUEUE_DESC desc{};desc.Type=D3D12_COMMAND_LIST_TYPE_COMPUTE;
    AmdNrTools::ComPtr<ID3D12CommandQueue> compute;
    AmdNrTools::CheckDeviceApi(context.device->CreateCommandQueue(&desc,IID_PPV_ARGS(&compute)));
    Require(!C512ProjectionProbe::Create(context.device.Get(),compute.Get()),"compute queue rejected");
    auto factory=AmdNrTools::Factory();
    for(UINT i=0;;++i) {
        AmdNrTools::ComPtr<IDXGIAdapter1> adapter;
        if(factory->EnumAdapters1(i,&adapter)==DXGI_ERROR_NOT_FOUND) break;
        DXGI_ADAPTER_DESC1 description{};adapter->GetDesc1(&description);
        if(description.Flags&DXGI_ADAPTER_FLAG_SOFTWARE) continue;
        AmdNrTools::ComPtr<ID3D12Device> other;
        if(SUCCEEDED(D3D12CreateDevice(adapter.Get(),D3D_FEATURE_LEVEL_11_0,IID_PPV_ARGS(&other))) && other.Get()!=context.device.Get()) {
            Require(!C512ProjectionProbe::Create(other.Get(),context.queue.Get()),"foreign device queue");break;
        }
    }
    const auto l=MakeC512TensorLayout({4,4}).value();
    std::vector<std::uint8_t> matrix(262144),input(l.ByteCount()),residual(l.ByteCount());
    for(unsigned k=0;k<512;++k) matrix[((37*k+11)%512)*512+k]=0x38;
    for(unsigned p=0;p<16;++p) for(unsigned k=0;k<512;++k) input[p*512+k]=std::uint8_t((p+k)%126+1|(((p+k)&1)<<7));
    const auto weights=Weights(matrix);const auto expected=Evaluate(l,input,residual,weights);
    auto job=probe->Submit(l,input,residual,weights);Require(bool(job),"projection submit");
    std::fill(input.begin(),input.end(),0);std::fill(residual.begin(),residual.end(),0x38);
    auto moved=std::move(*probe);
    Require(!probe->Readback(*job,std::chrono::milliseconds(0)),"moved-from context");
    const auto got=moved.Readback(*job,std::chrono::seconds(30));
    Require(bool(got) && *got==expected && got->size()==2*l.ByteCount(),"snapshot and separate readback count");
    Require(moved.Readback(*job,std::chrono::seconds(1)).value()==expected,"repeat readback");
    auto foreign=C512ProjectionProbe::Create(context.device.Get(),context.queue.Get());
    const auto wrong=foreign->Readback(*job,std::chrono::milliseconds(0));
    Require(!wrong && wrong.error()==ProbeError::InvalidJob,"foreign job");
    auto shortInput=moved.Submit(l,std::span(input).first(input.size()-1),{},weights);
    Require(!shortInput && shortInput.error()==ProbeError::Count,"short input");
    Require(!moved.Submit(l,input,std::span(residual).first(residual.size()-1),weights),"short residual");
    Require(!moved.Submit(MakeC512TensorLayout({20,16}).value(),input,{},weights),"excessive extent");
    auto invalidWeights=Weights(matrix);auto owner=std::move(invalidWeights);
    Require(!moved.Submit(l,input,{},invalidWeights),"moved weights");
    std::array<std::uint16_t,512> coef{};coef[0]=0x7c00;coef[1]=0xbc00;
    const auto special=Weights(matrix,coef);std::fill(residual.begin(),residual.end(),0);
    auto absent=moved.Submit(l,input,{},special);auto zeros=moved.Submit(l,input,residual,special);
    Require(bool(absent) && bool(zeros),"optional residual submissions");
    const auto zeroExpected=Evaluate(l,input,residual,special);
    Require(moved.Readback(*absent,std::chrono::seconds(30)).value()==zeroExpected &&
        moved.Readback(*zeros,std::chrono::seconds(30)).value()==zeroExpected,"absent residual is decoded positive zero, including zero times infinity");
    { auto dropped=moved.Submit(l,input,{},weights);Require(bool(dropped),"dropped handle submit"); }
    Require(bool(moved.Drain(std::chrono::seconds(30))),"dropped handle remains owned");
    AmdNrTools::ComPtr<ID3D12Fence> gate;
    AmdNrTools::CheckDeviceApi(context.device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&gate)));
    AmdNrTools::CheckDeviceApi(context.queue->Wait(gate.Get(),1));
    auto delayed=moved.Submit(l,input,{},weights);Require(bool(delayed),"gated submit");
    const auto timeout=moved.Readback(*delayed,std::chrono::milliseconds(0));
    // Release even if the timeout assertion fails, so no queued resources are stranded by this test.
    AmdNrTools::CheckDeviceApi(gate->Signal(1));
    Require(!timeout && timeout.error()==ProbeError::Timeout,"timeout preserves job");
    Require(moved.Readback(*delayed,std::chrono::seconds(30)).value()==Evaluate(l,input,{},weights),"delayed job readable");
    Require(bool(moved.Drain(std::chrono::seconds(30))),"drain after gate");context.CheckMessages();
    std::puts("PASS: portable C512 shader, snapshots, counts, ownership and queue timeout");
}
void FileOracle(bool cpu,bool warp,LUID luid,const wchar_t* inputPath,const wchar_t* outputPath) {
    // Test-only format: LE extent, canonical input/residual bytes, generated raw weight record.
    std::ifstream file(std::filesystem::path(inputPath),std::ios::binary);
    std::array<unsigned char,8> header{};
    if(!file.read(reinterpret_cast<char*>(header.data()),header.size())) throw std::runtime_error("short header");
    auto word=[&](unsigned at){return std::uint32_t(header[at])|std::uint32_t(header[at+1])<<8|std::uint32_t(header[at+2])<<16|std::uint32_t(header[at+3])<<24;};
    auto layout=MakeC512TensorLayout({word(0),word(4)});
    if(!layout || layout->ByteCount()>C512ProjectionMaxValues) throw std::runtime_error("invalid extent");
    std::vector<std::uint8_t> input(layout->ByteCount()),residual(layout->ByteCount());std::vector<std::byte> raw(263168);
    if(!file.read(reinterpret_cast<char*>(input.data()),input.size()) ||
       !file.read(reinterpret_cast<char*>(residual.data()),residual.size()) ||
       !file.read(reinterpret_cast<char*>(raw.data()),raw.size()) || file.peek()!=std::char_traits<char>::eof()) throw std::runtime_error("invalid fixture length");
    auto weights=DecodeC512Projection(raw);if(!weights) throw std::runtime_error("invalid record");
    std::vector<std::uint32_t> output;
    if(cpu) output=Evaluate(*layout,input,residual,*weights);
    else { GpuContext context(warp,luid);auto probe=C512ProjectionProbe::Create(context.device.Get(),context.queue.Get());
        if(!probe) throw std::runtime_error("probe creation");auto job=probe->Submit(*layout,input,residual,*weights);
        if(!job) throw std::runtime_error("submission");output=probe->Readback(*job,std::chrono::seconds(30)).value(); }
    std::ofstream result(std::filesystem::path(outputPath),std::ios::binary|std::ios::trunc);
    result.write(reinterpret_cast<const char*>(output.data()),output.size()*4);result.close();
    if(!result) throw std::runtime_error("output write");
}
int wmain(int argc,wchar_t** argv) try {
    if(argc>=2 && std::wstring_view(argv[1])==L"--oracle") {
        if(argc==5 && std::wstring_view(argv[2])==L"--cpu") FileOracle(true,false,{},argv[3],argv[4]);
        else if(argc==5 && std::wstring_view(argv[2])==L"--warp") FileOracle(false,true,{},argv[3],argv[4]);
        else if(argc==6 && std::wstring_view(argv[2])==L"--adapter-luid") FileOracle(false,false,AmdNrTools::ParseLuid(argv[3]),argv[4],argv[5]);
        else throw std::runtime_error("oracle arguments");return 0;
    }
    if(argc>1) {
        bool warp=std::wstring_view(argv[1])==L"--warp";
        bool debug=argc==3 && std::wstring_view(argv[2])==L"--debug";
        if(!(warp && (argc==2 || debug)) && !(argc==3 && std::wstring_view(argv[1])==L"--adapter-luid")) throw std::runtime_error("arguments");
        AmdNrTools::ComPtr<ID3D12Debug> layer;
        if(debug && FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&layer)))) {std::puts("UNAVAILABLE: D3D12 debug layer");return 77;}
        if(layer) layer->EnableDebugLayer();GpuTests(warp,warp?LUID{}:AmdNrTools::ParseLuid(argv[2]),debug);return 0;
    }
    AsymmetricPixelChannels();ResidualAndChunkBoundaries();ChunkRounding();EarlyResidualRounding();SpecialValues();ValidationBeforeWrites();
    std::puts("PASS: ordered C512 CPU reference, rounding boundaries, special values and validation");
    return 0;
} catch(const std::exception& error) {std::fprintf(stderr,"FAIL: %s\n",error.what());return 1;}
