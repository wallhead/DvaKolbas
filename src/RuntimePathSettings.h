#pragma once
#include "IniLayout.h"

#include "NvidiaBaselinePolicy.h"
#include <filesystem>
#include <string>

namespace TheosRenderPipeline
{
    struct RuntimePathSettings
    {
        std::string streamline;
        std::string neural;

        template<class Ini> void Load(const Ini& source)
        {
            const TheosRenderPipeline::IniLayout::ReadView ini(source);
            streamline = ini.GetValue("Experimental", "SourceDLSSGStreamlineDirectory", "");
            neural = ini.GetValue("Experimental", "NeuralRenderingRuntimePath", "");
        }

        [[nodiscard]] RuntimePathSettings Resolve(const std::filesystem::path& pluginDirectory) const
        {
            return {ResolveRuntimePath(streamline, pluginDirectory).string(),
                ResolveRuntimePath(neural, pluginDirectory).string()};
        }

        template<class Ini> void Store(Ini& ini) const
        {
            // No menu control edits these startup paths. Retain an on-disk
            // edit made during this session; only seed missing values.
            if (!ini.GetValue("Runtime", "StreamlineDirectory", nullptr)) {
                ini.SetValue("Runtime", "StreamlineDirectory", streamline.c_str());
            }
            if (!ini.GetValue("Runtime", "NRRuntimePath", nullptr)) {
                ini.SetValue("Runtime", "NRRuntimePath", neural.c_str());
            }
        }
    };
}
