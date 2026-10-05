#pragma once
#include "IniLayout.h"
#include "NeuralRenderingReconstruction.h"
#include "NeuralRenderingTuning.h"

namespace TheosRenderPipeline::NeuralRendering
{
	struct SecondPassSettings
	{
		bool linked{ true };
		float inputScale{ 1 };
		int preset{};
		Tuning tuning{};
		bool operator==(const SecondPassSettings&) const = default;
	};
	inline SecondPassSettings SanitizeSecondPass(SecondPassSettings value)
	{
		value.inputScale = NormalizeInputScale(value.inputScale);
		if (value.preset < 0 || value.preset > 1) { value.preset = 0; }
		value.tuning = SanitizeBuild14Tuning(value.tuning);
		return value;
	}
	inline SecondPassSettings EffectiveSecondPass(const SecondPassSettings& value, const Reconstruction& first, const Tuning& tuning)
	{
		return SanitizeSecondPass(value.linked ? SecondPassSettings{ true, first.inputScale, first.preset, tuning } : value);
	}
	template<class Ini> SecondPassSettings LoadPassSettings(const Ini& source, const char* section, const Reconstruction& first, const Tuning& tuning, int pass)
	{
		const TheosRenderPipeline::IniLayout::ReadView ini(source);
		const auto prefix = std::string("NRPass") + std::to_string(pass);
		// Missing overrides inherit the old shared settings. Relinking never erases
		// saved overrides, so experimenting with the checkbox is reversible.
		SecondPassSettings value;
		value.linked = ini.GetBoolValue(section, (prefix + "UseSameSettings").c_str(), true);
		value.inputScale = static_cast<float>(ini.GetDoubleValue(section, (prefix + "InputScale").c_str(), first.inputScale));
		value.preset = static_cast<int>(ini.GetLongValue(section, (prefix + "Preset").c_str(), first.preset));
		auto& t = value.tuning;
		t.style = static_cast<int>(ini.GetLongValue(section, (prefix + "Style").c_str(), tuning.style));
		t.intensity = static_cast<float>(ini.GetDoubleValue(section, (prefix + "Intensity").c_str(), tuning.intensity));
		t.localToneStrength = static_cast<float>(ini.GetDoubleValue(section, (prefix + "LocalTone").c_str(), tuning.localToneStrength));
		t.localStructureStrength = static_cast<float>(ini.GetDoubleValue(section, (prefix + "LocalStructure").c_str(), tuning.localStructureStrength));
		t.skinStructureStrength = static_cast<float>(ini.GetDoubleValue(section, (prefix + "SkinStructure").c_str(), tuning.skinStructureStrength));
		t.useAutoSkinMask = ini.GetBoolValue(section, (prefix + "AutoSkinMask").c_str(), tuning.useAutoSkinMask);
		t.uiCorrection = ini.GetBoolValue(section, (prefix + "UICorrection").c_str(), tuning.uiCorrection);
		return SanitizeSecondPass(value);
	}
	template<class Ini> void StorePassSettings(Ini& ini, const char* section, SecondPassSettings value, int pass)
	{
		value = SanitizeSecondPass(value);
		const auto prefix = std::string("NRPass") + std::to_string(pass);
		ini.SetBoolValue(section, (prefix + "UseSameSettings").c_str(), value.linked);
		ini.SetDoubleValue(section, (prefix + "InputScale").c_str(), value.inputScale);
		ini.SetLongValue(section, (prefix + "Preset").c_str(), value.preset);
		const auto& t = value.tuning;
		ini.SetLongValue(section, (prefix + "Style").c_str(), t.style);
		ini.SetDoubleValue(section, (prefix + "Intensity").c_str(), t.intensity);
		ini.SetDoubleValue(section, (prefix + "LocalTone").c_str(), t.localToneStrength);
		ini.SetDoubleValue(section, (prefix + "LocalStructure").c_str(), t.localStructureStrength);
		ini.SetDoubleValue(section, (prefix + "SkinStructure").c_str(), t.skinStructureStrength);
		ini.SetBoolValue(section, (prefix + "AutoSkinMask").c_str(), t.useAutoSkinMask);
		ini.SetBoolValue(section, (prefix + "UICorrection").c_str(), t.uiCorrection);
	}
	template<class Ini> SecondPassSettings LoadSecondPass(const Ini& source, const char* section, const Reconstruction& first, const Tuning& tuning)
	{
		return LoadPassSettings(source, section, first, tuning, 2);
	}
	template<class Ini> SecondPassSettings LoadThirdPass(const Ini& source, const char* section, const Reconstruction& first, const Tuning& tuning)
	{
		return LoadPassSettings(source, section, first, tuning, 3);
	}
	template<class Ini> void StoreSecondPass(Ini& ini, const char* section, SecondPassSettings value)
	{
		StorePassSettings(ini, section, value, 2);
	}
	template<class Ini> void StoreThirdPass(Ini& ini, const char* section, SecondPassSettings value)
	{
		StorePassSettings(ini, section, value, 3);
	}
}
