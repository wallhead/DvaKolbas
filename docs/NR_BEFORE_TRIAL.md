# First Skyrim NR Before trial

This trial uses one native SDR NR pass **before upscaling and frame generation**, with DLSS/DLAA or FSR. The installed trial preserves the user's upscaler and FG choices. After FG, reduced-model reconstruction and HDR NR are unavailable. The full NR plan remains incomplete.

Start Skyrim through the existing MO2 SKSE entry, load a save, and press End. Neural Rendering should report **NR active** on a world frame. Toggle NR off/on with Apply in that tab; `[` disables NR and `]` enables it for this session. Save as default persists the request, while Apply changes only the current session. No restart is needed for NR enabled or tuning changes. Profile/runtime paths stay fixed for the process.

Inspect motion, distant detail, lighting, skin and HUD while comparing the same scene off/on. Check inventory/map/dialogue, save/reload, one fast travel and alt-tab. Report a crash, black screen, persistent inactive NR, flicker, damaged HUD, unexpected color changes or failure to resume. Do not treat changed synthetic pixels as proof of good Skyrim image quality. The Gamma22 source setting follows the accepted FSR trial; the community NR model's visual/temporal suitability is awaiting this game test.

**Stable colors** defaults on for community NR Before. It preserves the original broad colors and brightness while retaining fine NR detail; it deliberately reduces NR tone/color grading and medium structure. Toggle it with Apply in the NR tab, without restarting. Save as default persists `[SourceDLSSG] NRStableColors=true/false`. Off uses the raw vendor result. See [tone stability](NR_TONE_STABILITY.md) for the regression and limitations. The log records the chosen mode and tuning in `[Community NR settings]` on startup and settings changes.

RTX 40 output was tested on RTX 4080 SUPER. New packages retain three logical profiles with two physical DLLs: RTX 40/50 share the exact `e67dee...` compatibility runtime at `NR/rtx40/nvngx_dlssnr.dll`; RTX 20/30 retain the supplied FP16 file. RTX 50 and RTX 20/30 hardware output is **NOT RUN**. AMD NR is unsupported. This local trial requires the exact currently qualified NVIDIA driver-core file; a different driver core is unavailable until checked. It does not silently load another DLL by basename. Both patched files use a narrowly checked in-memory caller-identity compatibility shim, including on RTX 50; source DLL bytes are preserved. Original downloads and historical rollback packages remain separate.

The source image must come from a confirmed native UI handoff. Menu/loading and fallback frames without a clean world boundary bypass NR. Resets cover camera discontinuities, off/on, source gaps and resize. A failure with unknown GPU ownership stops rendering and requires restart; the log preserves its reason.

Keep previous TRP and AIO mods for rollback. Installation has a separate mod folder and a backup of the exact profile mod list; do not run two upscaler mods together. Logs contain `[Community NR startup]` and `[Community NR frame]` with requested/effective state, selected profile, source ID, revision and reset/evaluation counts. Runtime files stay local; Git contains implementation and evidence only.


The later [opt-in SDR-byte comparison](../research/nr/tone-contract/SDR_BYTES_TRIAL.md)
requires `[NeuralRendering] SdrBytesTrial=true` at startup and Stable colors
off. It keeps encoded RGBA8 through NR and lets FSR decode afterward. Full tone
and live style/off/on controls remain available. This is an isolated comparison
against user-confirmed stable Raz gameplay; default FP16 behavior is retained.
