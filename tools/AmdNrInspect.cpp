#include "NeuralRendering/Amd/ModelIdentity.h"
#include "NeuralRendering/Amd/ModelGeometry.h"
#include "NeuralRendering/Amd/C512ProjectionArchive.h"
#include "AmdNrFormatFiles.h"
#include <charconv>
#include <iostream>
#include <map>
#include <set>
using namespace TheosRenderPipeline::NeuralRendering::Amd;
static std::uint32_t Dimension(std::wstring_view value) {
    std::string narrow;
    for(auto c:value) {if(c<L'0' || c>L'9') throw std::runtime_error("invalid dimension");narrow+=char(c);}
    std::uint32_t n{};auto [end,error]=std::from_chars(narrow.data(),narrow.data()+narrow.size(),n);
    if(error!=std::errc{} || end!=narrow.data()+narrow.size() || !n) throw std::runtime_error("invalid dimension");return n;
}
static const char* ProjectionErrorText(ProjectionError error) noexcept {
    switch(error) {
    case ProjectionError::RecordSize:return "projection record has an unexpected size";
    case ProjectionError::Memory:return "projection allocation failed";
    case ProjectionError::UnsupportedArchive:return "projection archive identity is unsupported";
    case ProjectionError::UnsupportedSelection:return "projection block/layer is unsupported";
    case ProjectionError::Archive:return "projection record is missing or invalid";
    case ProjectionError::Crypto:return "projection archive hash failed";
    }
    return "projection decode failed";
}
int wmain(int argc,wchar_t** argv) {
    try {
        std::map<std::wstring,std::wstring> options;
        const std::set<std::wstring> flags{L"--help",L"--no-extra-height",L"--list"};
        const std::set<std::wstring> values{L"--weights",L"--width",L"--height",L"--mode",L"--format",L"--input",L"--output",L"--projection-block",L"--projection-layer"};
        for(int i=1;i<argc;++i) {
            std::wstring key=argv[i];if(options.contains(key)) throw std::runtime_error("duplicate option");
            if(flags.contains(key)) options[key]=L"";
            else if(values.contains(key) && i+1<argc) options[key]=argv[++i];
            else throw std::runtime_error("unknown option or missing value");
        }
        if(options.contains(L"--help")) {
            if(options.size()!=1) throw std::runtime_error("help takes no other options");
            std::cout<<"--weights PATH --width U32 --height U32 [--mode default|8|128] [--no-extra-height] [--list] [--projection-block U32 --projection-layer U32]\n--format f32-to-f16|f16-to-f32|e4m3-to-f16 --input PATH --output PATH\n";return 0;
        }
        const auto need=[&](const wchar_t* key)->const std::wstring& {auto i=options.find(key);if(i==options.end()) throw std::runtime_error("missing required option");return i->second;};
        if(options.contains(L"--format")) {
            if(options.size()!=3) throw std::runtime_error("format mode requires exactly three options");
            auto op=AmdNrTools::ParseFormat(need(L"--format"));
            std::filesystem::path input=need(L"--input"),output=need(L"--output");AmdNrTools::CheckDistinct(input,output);
            auto words=AmdNrTools::ReadWords(input);std::vector<std::uint32_t> converted(words.size());
            if(!ConvertFormatWords(op,words,converted)) throw std::runtime_error("conversion failed");
            AmdNrTools::PublishWords(output,converted);return 0;
        }
        if(options.contains(L"--input") || options.contains(L"--output")) throw std::runtime_error("unexpected file-mode option");
        const bool projectionRequested=options.contains(L"--projection-block") || options.contains(L"--projection-layer");
        std::uint32_t projectionBlock{},projectionLayer{};
        if(projectionRequested) {
            projectionBlock=Dimension(need(L"--projection-block"));projectionLayer=Dimension(need(L"--projection-layer"));
            if(!IsC512ProjectionSelection(projectionBlock,projectionLayer)) throw std::runtime_error("unsupported projection selection");
        }
        ExtentMode mode=ExtentMode::Default;
        if(options.contains(L"--mode")) {auto& m=options.at(L"--mode");if(m==L"8") mode=ExtentMode::Step8;else if(m==L"128") mode=ExtentMode::Step128;else if(m!=L"default") throw std::runtime_error("invalid mode");}
        auto geometry=MakeGeometry({Dimension(need(L"--width")),Dimension(need(L"--height"))},mode,options.contains(L"--no-extra-height"));
        if(!geometry) throw std::runtime_error("invalid processing extent");
        auto file=ReadArchive(need(L"--weights"));if(!file) throw std::runtime_error(file.error().message);
        auto id=IdentifyModel(*file);if(!id) throw std::runtime_error("archive hash failed");
        bool known=id->status==ModelStatus::KnownArchiveIncompleteSchema;
        std::cout<<"status="<<(known?"KnownArchiveIncompleteSchema":"UnsupportedArchive")<<"\nbytes="<<file->Bytes().size()<<"\nrecords="<<file->Index().records.size()<<"\nsha256=";
        const char* hex="0123456789abcdef";for(auto b:id->digest){auto n=std::to_integer<unsigned>(b);std::cout<<hex[n>>4]<<hex[n&15];}
        std::cout<<"\nprocessing="<<geometry->processing.width<<'x'<<geometry->processing.height<<"\ndeep="<<geometry->levels[5].extent.width<<'x'<<geometry->levels[5].extent.height<<"\ninference=unavailable\n";
        if(options.contains(L"--list")) for(auto& r:file->Index().records) std::cout<<r.name<<" offset="<<r.offset<<" bytes="<<r.size<<'\n';
        if(known && projectionRequested) {
            auto projection=ReadKnownC512Projection(*file,projectionBlock,projectionLayer);
            if(!projection) throw std::runtime_error(ProjectionErrorText(projection.error()));
            std::cout<<"projection_record=block"<<projectionBlock<<".layer"<<projectionLayer<<".layer\nprojection_basis=native-fragment\nprojection_matrix_codes="
                <<projection->MatrixCodes().size()<<"\nprojection_residual_halves="<<projection->ResidualHalfBits().size()<<'\n';
        }
        return known?0:3;
    } catch(const std::exception& e) {std::cerr<<"error: "<<e.what()<<'\n';return 2;}
}
