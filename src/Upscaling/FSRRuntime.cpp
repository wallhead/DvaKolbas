#include "FSRRuntime.h"
#include <ffx_upscale.h>
#include <dx12/ffx_api_dx12.h>
#include <fstream>
#include <optional>
#ifdef TRP_ENABLE_FSR_FG
#include <ffx_framegeneration.h>
#include <dx12/ffx_api_framegeneration_dx12.h>
#endif

namespace TheosRenderPipeline::Upscaling
{
    namespace
    {
        RuntimeError Error(ErrorKind kind, std::int64_t code, std::string message)
        { return {kind, code, std::move(message)}; }
        RuntimeError QueryError(ffxReturnCode_t code)
        {
            const auto kind = code == FFX_API_RETURN_NO_PROVIDER ? ErrorKind::NoProvider :
                code == FFX_API_RETURN_ERROR_UNKNOWN_DESCTYPE || code == FFX_API_RETURN_PROVIDER_NO_SUPPORT_NEW_DESCTYPE ?
                    ErrorKind::IncompatibleAbi : ErrorKind::UnsupportedDevice;
            return Error(kind, code, "FidelityFX provider query failed");
        }
        Result<void> CheckModuleFile(const std::filesystem::path& path)
        {
            std::ifstream file(path, std::ios::binary);
            if (!file) return std::unexpected(Error(ErrorKind::MissingRuntime, ERROR_FILE_NOT_FOUND, "Missing FSR runtime: " + path.string()));
            IMAGE_DOS_HEADER dos{}; file.read(reinterpret_cast<char*>(&dos), sizeof(dos));
            if (!file || dos.e_magic != IMAGE_DOS_SIGNATURE || dos.e_lfanew < sizeof(dos))
                return std::unexpected(Error(ErrorKind::IncompatibleAbi, ERROR_BAD_EXE_FORMAT, "Invalid FSR PE header"));
            DWORD signature{}; IMAGE_FILE_HEADER header{};
            file.seekg(dos.e_lfanew); file.read(reinterpret_cast<char*>(&signature), sizeof(signature));
            file.read(reinterpret_cast<char*>(&header), sizeof(header));
            if (!file || signature != IMAGE_NT_SIGNATURE)
                return std::unexpected(Error(ErrorKind::IncompatibleAbi, ERROR_BAD_EXE_FORMAT, "Invalid FSR PE signature"));
            if (header.Machine != IMAGE_FILE_MACHINE_AMD64)
                return std::unexpected(Error(ErrorKind::WrongArchitecture, header.Machine, "FSR requires x64 runtime DLLs"));
            // Refuse a foreign loaded module with the same basename before the
            // effect loader can resolve a request to that module.
            if (const auto existing = GetModuleHandleW(path.filename().c_str())) {
                std::wstring name(32768, L'\0');
                const auto length = GetModuleFileNameW(existing, name.data(), static_cast<DWORD>(name.size()));
                if (!length || length >= name.size())
                    return std::unexpected(Error(ErrorKind::IncompatibleAbi, GetLastError(), "Cannot identify loaded FSR runtime"));
                name.resize(length); std::error_code ec;
                if (!std::filesystem::equivalent(path, name, ec) || ec)
                    return std::unexpected(Error(ErrorKind::IncompatibleAbi, 0, "A different FSR runtime with this filename is already loaded"));
            }
            return {};
        }
        Result<std::string> CopyName(const char* name)
        {
            if (!name) return std::unexpected(Error(ErrorKind::IncompatibleAbi, 0, "FSR returned a null provider name"));
            const auto length = strnlen_s(name, 4096);
            if (!length || length == 4096) return std::unexpected(Error(ErrorKind::IncompatibleAbi, 0, "Invalid FSR provider name"));
            return std::string(name, length);
        }
        std::optional<uint32_t> QualityMode(Quality value)
        {
            switch (value) {
            case Quality::Quality: return FFX_UPSCALE_QUALITY_MODE_QUALITY;
            case Quality::Balanced: return FFX_UPSCALE_QUALITY_MODE_BALANCED;
            case Quality::Performance: return FFX_UPSCALE_QUALITY_MODE_PERFORMANCE;
            case Quality::NativeAA: return FFX_UPSCALE_QUALITY_MODE_NATIVEAA;
            }
            return {};
        }
    }

    Result<FsrEffectProvider> SelectFsrEffectProvider(const std::vector<FsrEffectProvider>& providers, FsrEffect effect)
    {
        // Observed using the verified v2.3.0 binaries on the matching adapter.
        // Opaque IDs are compared as identities, never decoded into versions.
        const ProviderInfo* expected{};
        static const ProviderInfo generation{17726168133342859270ull, "3.1.6"};
        static const ProviderInfo swapchain{17752306900579389447ull, "3.1.7"};
        if (effect == FsrEffect::FrameGeneration) expected = &generation;
        if (effect == FsrEffect::FrameGenerationSwapChain) expected = &swapchain;
        if (!expected) return std::unexpected(Error(ErrorKind::InvalidInput, 0, "FG selection requires an FG or swapchain effect; SR uses its existing provider policy"));
        for (const auto& provider : providers) {
            if (provider.effect == effect && provider.identity.id == expected->id && provider.identity.name == expected->name)
                return provider;
        }
        return std::unexpected(Error(ErrorKind::NoProvider, 0, "Pinned analytical FG/swapchain provider identity is absent; no implicit fallback"));
    }

    FsrRuntime::~FsrRuntime() { Unload(); }
    void FsrRuntime::Unload()
    {
        functions_ = {};
        if (loader_) { FreeLibrary(loader_); loader_ = nullptr; }
        if (frameGeneration_) { FreeLibrary(frameGeneration_); frameGeneration_ = nullptr; }
        if (upscaler_) { FreeLibrary(upscaler_); upscaler_ = nullptr; }
    }
    Result<void> FsrRuntime::Load(const std::filesystem::path& pluginDirectory)
    {
        if (loader_ || upscaler_ || !pluginDirectory.is_absolute())
            return std::unexpected(Error(ErrorKind::InvalidInput, 0, "FSR requires an absolute plugin directory and an unloaded runtime"));
        std::error_code ec;
        const auto root = std::filesystem::weakly_canonical(pluginDirectory / "FSR", ec);
        if (ec) return std::unexpected(Error(ErrorKind::MissingRuntime, ec.value(), "Cannot resolve FSR runtime directory"));
        const auto upscaler = root / "amd_fidelityfx_upscaler_dx12.dll";
        const auto loader = root / "amd_fidelityfx_loader_dx12.dll";
        for (const auto& path : {upscaler, loader}) {
            if (auto checked = CheckModuleFile(path); !checked) return checked;
        }
        constexpr DWORD flags = LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_SYSTEM32;
        // Preload the exact effect DLL before the loader resolves its basename.
        upscaler_ = LoadLibraryExW(upscaler.c_str(), nullptr, flags);
        if (upscaler_) loader_ = LoadLibraryExW(loader.c_str(), nullptr, flags);
        if (!loader_) {
            const auto error = GetLastError(); Unload();
            return std::unexpected(Error(error == ERROR_BAD_EXE_FORMAT ? ErrorKind::IncompatibleAbi : ErrorKind::MissingRuntime,
                error, "Cannot load the plugin-relative FidelityFX runtime or its dependencies"));
        }
        functions_.CreateContext = reinterpret_cast<PfnFfxCreateContext>(GetProcAddress(loader_, "ffxCreateContext"));
        functions_.DestroyContext = reinterpret_cast<PfnFfxDestroyContext>(GetProcAddress(loader_, "ffxDestroyContext"));
        functions_.Configure = reinterpret_cast<PfnFfxConfigure>(GetProcAddress(loader_, "ffxConfigure"));
        functions_.Query = reinterpret_cast<PfnFfxQuery>(GetProcAddress(loader_, "ffxQuery"));
        functions_.Dispatch = reinterpret_cast<PfnFfxDispatch>(GetProcAddress(loader_, "ffxDispatch"));
        if (!functions_.CreateContext || !functions_.DestroyContext || !functions_.Configure || !functions_.Query || !functions_.Dispatch) {
            Unload(); return std::unexpected(Error(ErrorKind::MissingExport, ERROR_PROC_NOT_FOUND, "FSR requires all five FidelityFX C exports"));
        }
        return {};
    }

    Result<void> FsrRuntime::LoadFrameGeneration(const std::filesystem::path& pluginDirectory)
    {
#ifdef TRP_ENABLE_FSR_FG
        if (!loader_ || frameGeneration_ || !pluginDirectory.is_absolute())
            return std::unexpected(Error(ErrorKind::InvalidInput, 0, "FG requires a loaded SR runtime, an absolute directory and no loaded FG module"));
        std::error_code ec;
        const auto path = std::filesystem::weakly_canonical(pluginDirectory / "FSR/amd_fidelityfx_framegeneration_dx12.dll", ec);
        if (ec) return std::unexpected(Error(ErrorKind::MissingRuntime, ec.value(), "Cannot resolve FG runtime path"));
        if (auto checked = CheckModuleFile(path); !checked) return checked;
        const auto module = LoadLibraryExW(path.c_str(), nullptr, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!module) return std::unexpected(Error(ErrorKind::MissingRuntime, GetLastError(), "Cannot load plugin-relative FidelityFX FG runtime or dependencies"));
        for (const auto* symbol : {"ffxCreateContext", "ffxDestroyContext", "ffxConfigure", "ffxQuery", "ffxDispatch"}) {
            if (!GetProcAddress(module, symbol)) {
                FreeLibrary(module);
                return std::unexpected(Error(ErrorKind::MissingExport, ERROR_PROC_NOT_FOUND, "FG requires all five FidelityFX C exports; SR remains loaded"));
            }
        }
        frameGeneration_ = module;
        return {};
#else
        return std::unexpected(Error(ErrorKind::NoProvider, 0, "This build does not include FSR frame generation"));
#endif
    }

    Result<std::vector<FsrEffectProvider>> FsrRuntime::EnumerateForEffect(ID3D12Device* device, FsrEffect effect)
    {
        uint64_t type{};
        switch (effect) {
        case FsrEffect::Upscale: type = FFX_API_CREATE_CONTEXT_DESC_TYPE_UPSCALE; break;
#ifdef TRP_ENABLE_FSR_FG
        case FsrEffect::FrameGeneration: type = FFX_API_CREATE_CONTEXT_DESC_TYPE_FRAMEGENERATION; break;
        case FsrEffect::FrameGenerationSwapChain: type = FFX_API_CREATE_CONTEXT_DESC_TYPE_FRAMEGENERATIONSWAPCHAIN_NEW_DX12; break;
#endif
        default: return std::unexpected(Error(ErrorKind::NoProvider, 0, "Unsupported FSR effect or build capability"));
        }
        if (effect != FsrEffect::Upscale && !frameGeneration_)
            return std::unexpected(Error(ErrorKind::MissingRuntime, 0, "FG enumeration requires explicit FG module loading"));
        auto discovered = EnumerateType(device, type);
        if (!discovered) return std::unexpected(discovered.error());
        std::vector<FsrEffectProvider> result; result.reserve(discovered->size());
        for (auto& identity : *discovered) result.push_back({effect, std::move(identity)});
        return result;
    }

    Result<std::vector<ProviderInfo>> FsrRuntime::Enumerate(ID3D12Device* device)
    { return EnumerateType(device, FFX_API_CREATE_CONTEXT_DESC_TYPE_UPSCALE); }

    Result<std::vector<ProviderInfo>> FsrRuntime::EnumerateType(ID3D12Device* device, uint64_t createDescType)
    {
        if (!functions_.Query || !device) return std::unexpected(Error(ErrorKind::InvalidInput, 0, "FSR enumeration needs a loaded runtime and actual D3D12 device"));
        ffxQueryDescGetVersions query{}; query.header.type = FFX_API_QUERY_DESC_TYPE_GET_VERSIONS;
        query.createDescType = createDescType; query.device = device;
        uint64_t count{}; query.outputCount = &count;
        auto result = functions_.Query(nullptr, &query.header);
        if (result != FFX_API_RETURN_OK) return std::unexpected(QueryError(result));
        // Counts can change between calls. Bound allocation and retry counts.
        for (unsigned attempt = 0; attempt < 4; ++attempt) {
            if (!count) return std::unexpected(Error(ErrorKind::NoProvider, 0, "No provider supports the requested FSR effect on this device"));
            if (count > 128) return std::unexpected(Error(ErrorKind::IncompatibleAbi, 0, "Invalid FSR provider count"));
            const auto capacity = count;
            std::vector<uint64_t> ids(static_cast<size_t>(capacity));
            std::vector<const char*> names(static_cast<size_t>(capacity));
            query.versionIds = ids.data(); query.versionNames = names.data();
            result = functions_.Query(nullptr, &query.header);
            if (result != FFX_API_RETURN_OK) return std::unexpected(QueryError(result));
            if (count > capacity) continue;
            if (!count) return std::unexpected(Error(ErrorKind::NoProvider, 0, "FSR providers disappeared during enumeration"));
            std::vector<ProviderInfo> providers; providers.reserve(static_cast<size_t>(count));
            for (size_t i = 0; i < count; ++i) {
                auto name = CopyName(names[i]); if (!name) return std::unexpected(name.error());
                providers.push_back({ids[i], std::move(*name)});
            }
            return providers;
        }
        return std::unexpected(Error(ErrorKind::IncompatibleAbi, 0, "FSR provider count did not stabilize"));
    }

    Result<Extent> FsrRuntime::QueryRenderExtent(ID3D12Device* device, const ProviderInfo& provider, Quality quality, Extent display)
    {
        const auto mode = QualityMode(quality);
        if (!functions_.Query || !device || !mode || !display.width || !display.height)
            return std::unexpected(Error(ErrorKind::InvalidInput, 0, "Invalid FSR sizing input"));
        ffxCreateBackendDX12Desc backend{{FFX_API_CREATE_CONTEXT_DESC_TYPE_BACKEND_DX12, nullptr}, device};
        ffxCreateContextDescUpscaleVersion version{{FFX_API_CREATE_CONTEXT_DESC_TYPE_UPSCALE_VERSION, nullptr}, FFX_UPSCALER_VERSION};
        ffxOverrideVersion override{{FFX_API_DESC_TYPE_OVERRIDE_VERSION, nullptr}, provider.id};
        backend.header.pNext = &version.header; version.header.pNext = &override.header;
        Extent render{};
        ffxQueryDescUpscaleGetRenderResolutionFromQualityMode query{};
        query.header = {FFX_API_QUERY_DESC_TYPE_UPSCALE_GETRENDERRESOLUTIONFROMQUALITYMODE, &backend.header};
        query.displayWidth = display.width; query.displayHeight = display.height; query.qualityMode = *mode;
        query.pOutRenderWidth = &render.width; query.pOutRenderHeight = &render.height;
        const auto result = functions_.Query(nullptr, &query.header);
        if (result != FFX_API_RETURN_OK) return std::unexpected(QueryError(result));
        if (!render.width || !render.height || render.width > display.width || render.height > display.height ||
            (quality == Quality::NativeAA && render != display))
            return std::unexpected(Error(ErrorKind::IncompatibleAbi, 0, "FSR provider returned an invalid render extent"));
        return render;
    }

    Result<ProviderInfo> FsrRuntime::QueryActualProvider(ffxContext& context)
    {
        if (!functions_.Query || !context) return std::unexpected(Error(ErrorKind::InvalidInput, 0, "No live FSR context"));
        ffxQueryGetProviderVersion query{}; query.header.type = FFX_API_QUERY_DESC_TYPE_GET_PROVIDER_VERSION;
        const auto result = functions_.Query(&context, &query.header);
        if (result != FFX_API_RETURN_OK) return std::unexpected(QueryError(result));
        auto name = CopyName(query.versionName); if (!name) return std::unexpected(name.error());
        return ProviderInfo{query.versionId, std::move(*name)};
    }
    Result<void> FsrRuntime::VerifyActualProvider(ffxContext& context, const ProviderInfo& expected)
    {
        const auto actual = QueryActualProvider(context); if (!actual) return std::unexpected(actual.error());
        if (actual->id != expected.id) return std::unexpected(Error(ErrorKind::ContextFailure, 0, "Created FSR provider differs from sizing provider"));
        return {};
    }
    Result<void> FsrRuntime::VerifyActualProvider(ffxContext& context, const FsrEffectProvider& expected)
    {
        if (auto valid = SelectFsrEffectProvider({expected}, expected.effect); !valid)
            return std::unexpected(valid.error());
        auto actual = QueryActualProvider(context);
        if (!actual) return std::unexpected(actual.error());
        if (actual->id != expected.identity.id || actual->name != expected.identity.name)
            return std::unexpected(Error(ErrorKind::ContextFailure, 0, "Created FG provider ID/name differs from its selected effect catalog"));
        return {};
    }
}
