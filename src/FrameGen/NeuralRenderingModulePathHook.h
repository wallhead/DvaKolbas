#pragma once

#include <Windows.h>

#include <cstddef>
#include <filesystem>
#include <memory>

namespace TheosRenderPipeline::NeuralRendering
{
	class ModulePathHook
	{
	public:
		ModulePathHook();
		~ModulePathHook();

		ModulePathHook(const ModulePathHook&) = delete;
		ModulePathHook& operator=(const ModulePathHook&) = delete;
		ModulePathHook(ModulePathHook&&) = delete;
		ModulePathHook& operator=(ModulePathHook&&) = delete;

		// Substitute only this proxy's containing caller module; all other module
		// queries keep their real paths. ANSI paths must be losslessly representable.
		bool Install(HMODULE a_featureModule, const std::filesystem::path& a_normalLoaderPath);
		// Failed restoration retains the remaining imports, callback data and both
		// module references. Retry succeeds only while those imports are still ours.
		bool Restore();

	private:
		struct State;
		std::unique_ptr<State> state_;
	};
}
