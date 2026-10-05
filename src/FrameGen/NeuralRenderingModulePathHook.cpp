#include "NeuralRenderingModulePathHook.h"

#include <PCH.h>

#include <delayimp.h>

#include <algorithm>
#include <cstring>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

namespace TheosRenderPipeline::NeuralRendering
{
	namespace
	{
		struct PatchedImport
		{
			void** slot{ nullptr };
			void* original{ nullptr };
			void* replacement{ nullptr };
		};

		struct ActiveHook
		{
			HMODULE featureModule{ nullptr };
			HMODULE retainedFeature{ nullptr };
			HMODULE callerModule{ nullptr };
			std::wstring normalLoaderPathW;
			std::string normalLoaderPathA;
			std::vector<PatchedImport> patches;
			std::size_t users{ 0 };
			bool restorePending{ false };
		};

		std::mutex g_hookMutex;
		ActiveHook g_activeHook;

		std::optional<std::string> ToAnsi(const std::wstring& a_text)
		{
			if (a_text.empty()) {
				return std::nullopt;
			}
			const auto codePage = ::GetACP();
			const auto flags = codePage == CP_UTF8 ? WC_ERR_INVALID_CHARS : WC_NO_BEST_FIT_CHARS;
			BOOL usedDefault = FALSE;
			auto* defaultResult = codePage == CP_UTF8 ? nullptr : &usedDefault;
			const auto required = ::WideCharToMultiByte(
				codePage, flags, a_text.c_str(), -1, nullptr, 0, nullptr, defaultResult);
			if (required <= 1 || usedDefault) {
				return std::nullopt;
			}
			std::string result(static_cast<std::size_t>(required), '\0');
			if (::WideCharToMultiByte(
					codePage, flags, a_text.c_str(), -1, result.data(), required, nullptr, defaultResult) != required || usedDefault) {
				return std::nullopt;
			}
			result.pop_back();
			return result;
		}

		template <class Char>
		DWORD CopyModulePath(
			const std::basic_string<Char>& a_path,
			Char* a_destination,
			DWORD a_capacity)
		{
			if (!a_destination || a_capacity == 0 || a_path.empty()) {
				return 0;
			}
			if (a_path.size() >= a_capacity) {
				const auto copied = static_cast<std::size_t>(a_capacity - 1);
				std::copy_n(a_path.data(), copied, a_destination);
				a_destination[copied] = Char{};
				::SetLastError(ERROR_INSUFFICIENT_BUFFER);
				return a_capacity;
			}
			std::copy(a_path.begin(), a_path.end(), a_destination);
			a_destination[a_path.size()] = Char{};
			return static_cast<DWORD>(a_path.size());
		}

		DWORD WINAPI HookGetModuleFileNameW(
			HMODULE a_module,
			LPWSTR a_filename,
			DWORD a_capacity)
		{
			const auto originalResult = ::GetModuleFileNameW(a_module, a_filename, a_capacity);
			std::scoped_lock lock(g_hookMutex);
			if (!a_module || !a_filename || a_capacity == 0 ||
				a_module != g_activeHook.callerModule || g_activeHook.normalLoaderPathW.empty()) {
				return originalResult;
			}
			return CopyModulePath(g_activeHook.normalLoaderPathW, a_filename, a_capacity);
		}

		DWORD WINAPI HookGetModuleFileNameA(
			HMODULE a_module,
			LPSTR a_filename,
			DWORD a_capacity)
		{
			const auto originalResult = ::GetModuleFileNameA(a_module, a_filename, a_capacity);
			std::scoped_lock lock(g_hookMutex);
			if (!a_module || !a_filename || a_capacity == 0 ||
				a_module != g_activeHook.callerModule || g_activeHook.normalLoaderPathA.empty()) {
				return originalResult;
			}
			return CopyModulePath(g_activeHook.normalLoaderPathA, a_filename, a_capacity);
		}

		bool ReplacePointer(void** a_slot, void* a_replacement, void** a_original)
		{
			if (!a_slot || !a_replacement || !a_original) {
				return false;
			}
			DWORD oldProtection = 0;
			if (!::VirtualProtect(a_slot, sizeof(*a_slot), PAGE_READWRITE, &oldProtection)) {
				return false;
			}
			*a_original = *a_slot;
			const auto previous = ::InterlockedCompareExchangePointer(a_slot, a_replacement, *a_original);
			DWORD ignored = 0;
			::VirtualProtect(a_slot, sizeof(*a_slot), oldProtection, &ignored);
			return previous == *a_original;
		}

		bool RestorePointer(const PatchedImport& a_patch)
		{
			if (!a_patch.slot || !a_patch.original) {
				return false;
			}
			DWORD oldProtection = 0;
			if (!::VirtualProtect(
					a_patch.slot, sizeof(*a_patch.slot), PAGE_READWRITE, &oldProtection)) {
				return false;
			}
			const auto previous = ::InterlockedCompareExchangePointer(a_patch.slot, a_patch.original, a_patch.replacement);
			DWORD ignored = 0;
			::VirtualProtect(a_patch.slot, sizeof(*a_patch.slot), oldProtection, &ignored);
			return previous == a_patch.replacement;
		}

		bool IsTargetImport(const char* a_name, void** a_replacement)
		{
			if (!a_name || !a_replacement) {
				return false;
			}
			if (std::strcmp(a_name, "GetModuleFileNameW") == 0) {
				*a_replacement = reinterpret_cast<void*>(&HookGetModuleFileNameW);
				return true;
			}
			if (std::strcmp(a_name, "GetModuleFileNameA") == 0) {
				*a_replacement = reinterpret_cast<void*>(&HookGetModuleFileNameA);
				return true;
			}
			return false;
		}

		bool ImageHeaders(
			HMODULE a_module,
			std::byte** a_base,
			IMAGE_NT_HEADERS64** a_ntHeaders)
		{
			if (!a_module || !a_base || !a_ntHeaders) {
				return false;
			}
			auto* base = reinterpret_cast<std::byte*>(a_module);
			auto* dos = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
			if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew <= 0) {
				return false;
			}
			auto* nt = reinterpret_cast<IMAGE_NT_HEADERS64*>(base + dos->e_lfanew);
			if (nt->Signature != IMAGE_NT_SIGNATURE ||
				nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
				return false;
			}
			*a_base = base;
			*a_ntHeaders = nt;
			return true;
		}

		void PatchNamedImport(
			IMAGE_THUNK_DATA64* a_nameThunk,
			IMAGE_THUNK_DATA64* a_iatThunk,
			std::byte* a_base,
			std::vector<PatchedImport>& a_patches)
		{
			if (!a_nameThunk || !a_iatThunk || IMAGE_SNAP_BY_ORDINAL64(a_nameThunk->u1.Ordinal)) {
				return;
			}
			auto* import = reinterpret_cast<IMAGE_IMPORT_BY_NAME*>(
				a_base + a_nameThunk->u1.AddressOfData);
			void* replacement = nullptr;
			if (!IsTargetImport(reinterpret_cast<const char*>(import->Name), &replacement)) {
				return;
			}
			auto* slot = reinterpret_cast<void**>(&a_iatThunk->u1.Function);
			void* original = nullptr;
			if (ReplacePointer(slot, replacement, &original)) {
				a_patches.push_back({ slot, original, replacement });
			}
		}

		void PatchNormalImports(
			std::byte* a_base,
			IMAGE_NT_HEADERS64* a_ntHeaders,
			std::vector<PatchedImport>& a_patches)
		{
			const auto& directory =
				a_ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
			if (directory.VirtualAddress == 0 ||
				directory.Size < sizeof(IMAGE_IMPORT_DESCRIPTOR)) {
				return;
			}
			auto* descriptor = reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(
				a_base + directory.VirtualAddress);
			for (; descriptor->Name != 0; ++descriptor) {
				auto* iat = reinterpret_cast<IMAGE_THUNK_DATA64*>(
					a_base + descriptor->FirstThunk);
				auto* names = descriptor->OriginalFirstThunk == 0 ? nullptr :
					reinterpret_cast<IMAGE_THUNK_DATA64*>(
						a_base + descriptor->OriginalFirstThunk);
				if (!names) {
					continue;
				}
				for (; names->u1.AddressOfData != 0 && iat->u1.Function != 0; ++names, ++iat) {
					PatchNamedImport(names, iat, a_base, a_patches);
				}
			}
		}

		void PatchDelayImports(
			std::byte* a_base,
			IMAGE_NT_HEADERS64* a_ntHeaders,
			std::vector<PatchedImport>& a_patches)
		{
			const auto& directory =
				a_ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT];
			if (directory.VirtualAddress == 0 || directory.Size < sizeof(ImgDelayDescr)) {
				return;
			}
			auto* descriptor = reinterpret_cast<ImgDelayDescr*>(
				a_base + directory.VirtualAddress);
			for (; descriptor->rvaDLLName != 0; ++descriptor) {
				if ((descriptor->grAttrs & dlattrRva) == 0 ||
					descriptor->rvaIAT == 0 || descriptor->rvaINT == 0) {
					continue;
				}
				auto* iat = reinterpret_cast<IMAGE_THUNK_DATA64*>(
					a_base + descriptor->rvaIAT);
				auto* names = reinterpret_cast<IMAGE_THUNK_DATA64*>(
					a_base + descriptor->rvaINT);
				for (; names->u1.AddressOfData != 0; ++names, ++iat) {
					PatchNamedImport(names, iat, a_base, a_patches);
				}
			}
		}

		void ReleaseActiveHook()
		{
			const auto feature = g_activeHook.retainedFeature;
			const auto caller = g_activeHook.callerModule;
			g_activeHook = {};
			if (feature) { ::FreeLibrary(feature); }
			if (caller) { ::FreeLibrary(caller); }
		}
	}

	struct ModulePathHook::State
	{
		bool installed{ false };
	};

	ModulePathHook::ModulePathHook() : state_(std::make_unique<State>()) {}

	ModulePathHook::~ModulePathHook()
	{
		if (state_ && state_->installed) {
			Restore();
		}
	}

	bool ModulePathHook::Install(
		HMODULE a_featureModule,
		const std::filesystem::path& a_normalLoaderPath)
	{
		if (!state_ || state_->installed || !a_featureModule || a_normalLoaderPath.empty()) {
			return false;
		}
		std::byte* base = nullptr;
		IMAGE_NT_HEADERS64* ntHeaders = nullptr;
		if (!ImageHeaders(a_featureModule, &base, &ntHeaders)) {
			return false;
		}
		const auto pathW = a_normalLoaderPath.wstring();
		const auto pathA = ToAnsi(pathW);
		if (!pathA) {
			logger::error("[DLSSNR Source] loader path cannot be represented exactly in the Windows ANSI code page; import hook not installed");
			return false;
		}

		std::scoped_lock lock(g_hookMutex);
		if (g_activeHook.featureModule) {
			if (g_activeHook.restorePending || g_activeHook.featureModule != a_featureModule ||
				g_activeHook.normalLoaderPathW != pathW) {
				return false;
			}
			++g_activeHook.users;
			state_->installed = true;
			return true;
		}
		// Keep the target and this proxy's containing caller module resident until
		// every import has been restored. A failed restore retains both references.
		if (!::GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
				reinterpret_cast<LPCWSTR>(a_featureModule), &g_activeHook.retainedFeature) ||
			!::GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
				reinterpret_cast<LPCWSTR>(&HookGetModuleFileNameW), &g_activeHook.callerModule)) {
			ReleaseActiveHook();
			return false;
		}
		g_activeHook.featureModule = a_featureModule;
		g_activeHook.normalLoaderPathW = pathW;
		g_activeHook.normalLoaderPathA = *pathA;
		g_activeHook.users = 1;

		PatchNormalImports(base, ntHeaders, g_activeHook.patches);
		PatchDelayImports(base, ntHeaders, g_activeHook.patches);
		if (g_activeHook.patches.empty()) {
			ReleaseActiveHook();
			return false;
		}
		state_->installed = true;
		return true;
	}

	bool ModulePathHook::Restore()
	{
		if (!state_ || !state_->installed) {
			return false;
		}
		std::scoped_lock lock(g_hookMutex);
		if (g_activeHook.users > 1) {
			state_->installed = false;
			--g_activeHook.users;
			return true;
		}
		bool restored = true;
		for (auto it = g_activeHook.patches.rbegin(); it != g_activeHook.patches.rend(); ++it) {
			if (RestorePointer(*it)) { it->slot = nullptr; }
			else { restored = false; }
		}
		std::erase_if(g_activeHook.patches, [](const PatchedImport& patch) { return patch.slot == nullptr; });
		if (restored) {
			state_->installed = false;
			ReleaseActiveHook();
		} else {
			g_activeHook.restorePending = true;
		}
		return restored;
	}
}
