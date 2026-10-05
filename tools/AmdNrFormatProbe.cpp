#include "NeuralRendering/Amd/FormatProbe.h"
#include "AmdNrFormatFiles.h"
#include "AmdNrDevices.h"
#include <d3d12sdklayers.h>
#include <map>
#include <set>
using namespace TheosRenderPipeline::NeuralRendering::Amd;
int wmain(int argc,wchar_t** argv) {
    try {
        std::map<std::wstring,std::wstring> options;
        const std::set<std::wstring> flags{L"--help",L"--warp",L"--list-adapters"};
        const std::set<std::wstring> values{L"--adapter-luid",L"--format",L"--input",L"--output"};
        for(int i=1;i<argc;++i) {
            std::wstring key=argv[i];if(options.contains(key)) throw std::runtime_error("duplicate option");
            if(flags.contains(key)) options[key]=L"";
            else if(values.contains(key) && i+1<argc) options[key]=argv[++i];
            else throw std::runtime_error("unknown option or missing value");
        }
        if(options.contains(L"--help")) {if(options.size()!=1) throw std::runtime_error("help takes no other options");std::cout<<"--warp|--adapter-luid HIGH:LOW --format f32-to-f16|f16-to-f32|e4m3-to-f16 --input PATH --output PATH\n--list-adapters\n";return 0;}
        if(options.contains(L"--list-adapters")) {if(options.size()!=1) throw std::runtime_error("list takes no other options");AmdNrTools::ListAdapters();return 0;}
        bool warp=options.contains(L"--warp"),explicitLuid=options.contains(L"--adapter-luid");
        if(warp==explicitLuid || options.size()!=4 || !options.contains(L"--format") || !options.contains(L"--input") || !options.contains(L"--output")) throw std::runtime_error("explicit adapter and three file-mode options required");
        auto operation=AmdNrTools::ParseFormat(options.at(L"--format"));
        auto input=std::filesystem::path(options.at(L"--input")),output=std::filesystem::path(options.at(L"--output"));
        AmdNrTools::CheckDistinct(input,output);auto words=AmdNrTools::ReadWords(input);
        if(words.size()>65535u*64) throw std::runtime_error("probe dispatch exceeds 65535 groups");
        auto adapter=AmdNrTools::SelectAdapter(warp,explicitLuid?AmdNrTools::ParseLuid(options.at(L"--adapter-luid")):LUID{});AmdNrTools::Describe(adapter.Get());
        AmdNrTools::ComPtr<ID3D12Debug> debug;bool validation=SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)));
        if(validation) debug->EnableDebugLayer();std::cout<<"debug_layer="<<(validation?"enabled":"unavailable")<<'\n';
        AmdNrTools::ComPtr<ID3D12Device> device;AmdNrTools::CheckDeviceApi(D3D12CreateDevice(adapter.Get(),D3D_FEATURE_LEVEL_11_0,IID_PPV_ARGS(&device)));
        D3D12_COMMAND_QUEUE_DESC desc{};desc.Type=D3D12_COMMAND_LIST_TYPE_DIRECT;
        AmdNrTools::ComPtr<ID3D12CommandQueue> queue;AmdNrTools::CheckDeviceApi(device->CreateCommandQueue(&desc,IID_PPV_ARGS(&queue)));
        auto probe=FormatProbe::Create(device.Get(),queue.Get());if(!probe) throw std::runtime_error("probe creation failed");
        auto job=probe->Submit(operation,words);if(!job) throw std::runtime_error("probe submission failed");
        auto converted=probe->Readback(*job,std::chrono::seconds(10));if(!converted || !probe->Drain(std::chrono::seconds(10))) throw std::runtime_error("GPU readback/retirement failed");
        AmdNrTools::PublishWords(output,*converted);std::cout<<"words="<<converted->size()<<'\n';return 0;
    } catch(const std::exception& e) {std::cerr<<"error: "<<e.what()<<'\n';return 2;}
}
