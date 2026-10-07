#include "FrameGen/SourceDLSSGSettings.h"
#include "FrameGen/SourceDLSSGNeuralState.h"
#include <SimpleIni.h>
#include <cstdio>
#include <cstdlib>
#include <limits>

static void Require(bool value, const char* why)
{
	if (!value) { std::fprintf(stderr, "FAIL: %s\n", why); std::exit(1); }
}
int main()
{
	using namespace TheosRenderPipeline;
	using namespace SourceDLSSG;
	CSimpleIniA ini;
	Require(LoadPreferences(ini).neuralPasses == 1, "missing pass count keeps the original one-pass default");
	Require(ini.LoadData("[NeuralRendering]\nPassCount=2\n[NR PASS 1]\nInputScale=0.75\nPreset=1\nIntensity=0.4\n") >= 0, "old INI");
	auto old = LoadPreferences(ini);
	Require(old.neuralPasses == 2, "old two-pass preference is preserved");
	Require(!old.neuralCombat.Enabled() && old.neuralCombat.recoverySeconds == 5, "old INI keeps combat policy off");
	old.neuralCombat = {true, true, 7.5f};
	Require(old.neuralSecondPass.linked && old.neuralSecondPass.inputScale == .75f && old.neuralSecondPass.preset == 1 &&
		old.neuralSecondPass.tuning.intensity == .4f, "old config inherits first pass and stays linked");
	Require(old.neuralThirdPass.linked && old.neuralThirdPass.inputScale == .75f && old.neuralThirdPass.preset == 1 &&
		old.neuralThirdPass.tuning.intensity == .4f, "old config keeps pass three linked to the first pass");
	old.neuralSecondPass.linked = false;
	old.neuralSecondPass.inputScale = .25f;
	old.neuralSecondPass.preset = 0;
	old.neuralSecondPass.tuning = { 7, .25f, 1.5f, .3f, -1, true, true };
	old.neuralPasses = 3;
	old.neuralThirdPass = { false, .5f, 1, { 2, .7f, .6f, .8f, .2f, false, true } };
	IniLayout::PrepareForUpdate(ini);
	StorePreferences(ini, old);
	IniLayout::StoreCanonical(ini);
	Require(LoadPreferences(ini) == old && LoadPreferences(ini).neuralPasses == 3,
		"three requested passes and all custom settings round trip independently");
	NeuralOptions legacy; legacy.passes = old.neuralPasses;
	const auto bounded = SanitizeNeuralOptions(legacy);
	Require(bounded.passes == 2 && bounded.EffectivePasses() == 2,
		"legacy execution never creates a third pass for a saved community request");
	IniLayout::PrepareForUpdate(ini);
	StorePreferences(ini, old);
	IniLayout::StoreCanonical(ini);
	Require(LoadPreferences(ini) == old,
		"saving after legacy execution keeps all three requested passes and custom overrides");
	old.neuralSecondPass.linked = true;
	old.neuralThirdPass.linked = true;
	IniLayout::PrepareForUpdate(ini);
	StorePreferences(ini, old);
	IniLayout::StoreCanonical(ini);
	auto linked = LoadPreferences(ini);
	Require(linked == old, "relink preserves saved overrides");
	const auto effective = NeuralRendering::EffectiveSecondPass(linked.neuralSecondPass, linked.neuralReconstruction, linked.neuralTuning);
	Require(effective.inputScale == .75f && effective.preset == 1 && effective.tuning == linked.neuralTuning, "link overrides hidden custom values");
	const auto third = NeuralRendering::EffectiveSecondPass(linked.neuralThirdPass, linked.neuralReconstruction, linked.neuralTuning);
	Require(third.inputScale == .75f && third.preset == 1 && third.tuning == linked.neuralTuning,
		"pass-three link follows first pass while retaining its independent stored overrides");
	linked.neuralThirdPass.linked = false;
	IniLayout::PrepareForUpdate(ini);
	StorePreferences(ini, linked);
	IniLayout::StoreCanonical(ini);
	Require(LoadPreferences(ini).neuralThirdPass == linked.neuralThirdPass &&
		LoadPreferences(ini).neuralThirdPass.tuning.intensity == .7f, "unlink restores the saved third-pass overrides");
	NeuralOptions options; options.enabled = true; options.passes = 2; options.secondPass = old.neuralSecondPass;
	options.secondPass.linked = false; options.worldOnly = true;
	Require(!options.EffectiveSecond().tuning.uiCorrection && options.secondPass.tuning.uiCorrection, "world-only adapter suppresses UI correction without changing preference");
	NeuralHistory history;
	Require(history.ResetFor(options, true, false), "initial history");
	Require(!history.ResetFor(options, true, false), "stable history");
	options.secondPass.tuning.intensity = .8f;
	Require(history.ResetFor(options, true, false), "second tuning resets history");
	options.secondPass.linked = true;
	Require(history.ResetFor(options, true, false), "relink resets history");
	options.passOverride = NeuralRendering::PassOverride::Combat;
	Require(options.passes == 2 && options.EffectivePasses() == 1 && history.ResetFor(options, true, false),
		"override changes execution and resets image history without changing requested passes");
	options.passOverride = NeuralRendering::PassOverride::WeaponsDrawn;
	Require(!history.ResetFor(options, true, false), "reason-only changes preserve temporal history");
	options.passOverride = NeuralRendering::PassOverride::Recovery;
	Require(!history.ResetFor(options, true, false), "cooldown preserves temporal history");
	IniLayout::PrepareForUpdate(ini);
	StorePreferences(ini, old);
	IniLayout::StoreCanonical(ini);
	Require(LoadPreferences(ini) == old && LoadPreferences(ini).neuralPasses == 3,
		"saving while overridden preserves requested passes and independent tuning");
	options.passOverride = NeuralRendering::PassOverride::None;
	Require(options.EffectivePasses() == 2 && history.ResetFor(options, true, false), "restoration resets image history");
	options.combat = {true, false, 2};
	Require(!history.ResetFor(options, true, false), "policy settings alone do not reset image history");
	old.neuralSecondPass.inputScale = std::numeric_limits<float>::quiet_NaN();
	old.neuralSecondPass.preset = 99; old.neuralSecondPass.tuning.intensity = std::numeric_limits<float>::infinity();
	old.neuralThirdPass.inputScale = std::numeric_limits<float>::infinity();
	old.neuralThirdPass.preset = -1; old.neuralThirdPass.tuning.intensity = std::numeric_limits<float>::quiet_NaN();
	old = SanitizePreferences(old);
	Require(old.neuralSecondPass.inputScale == 1 && old.neuralSecondPass.preset == 0 && old.neuralSecondPass.tuning.intensity == 1, "invalid custom settings sanitized");
	Require(old.neuralThirdPass.inputScale == 1 && old.neuralThirdPass.preset == 0 && old.neuralThirdPass.tuning.intensity == 1,
		"invalid pass-three custom settings sanitized");
	old.neuralPasses = 0;
	Require(SanitizePreferences(old).neuralPasses == 1, "zero passes sanitizes to the first pass");
	old.neuralPasses = 99;
	Require(SanitizePreferences(old).neuralPasses == 3, "excessive pass count sanitizes to three passes");
	std::puts("PASS: legacy defaults, independent INI persistence, relink preservation, sanitization and history/UI policy");
}
