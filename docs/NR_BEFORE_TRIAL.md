# First Skyrim NR Before trial

This trial uses one native SDR NR pass **before upscaling and frame generation**. FSR Native AA, analytical provider and FSR FG remain enabled. After FG, reduced-model reconstruction and HDR NR are unavailable. The full NR plan remains incomplete.

Start Skyrim through the existing MO2 SKSE entry, load a save, and press End. Neural Rendering should report **NR active** on a world frame. Toggle NR off/on with Apply in that tab; `[` disables NR and `]` enables it for this session. Save as default persists the request, while Apply changes only the current session. No restart is needed for NR enabled or tuning changes. Profile/runtime paths stay fixed for the process.

Inspect motion, distant detail, lighting, skin and HUD while comparing the same scene off/on. Check inventory/map/dialogue, save/reload, one fast travel and alt-tab. Report a crash, black screen, persistent inactive NR, flicker, damaged HUD, unexpected color changes or failure to resume. Do not treat changed synthetic pixels as proof of good Skyrim image quality. The Gamma22 source setting follows the accepted FSR trial; the community NR model's visual/temporal suitability is awaiting this game test.

RTX 40 output was tested on RTX 4080 SUPER. RTX 50 and RTX 20/30 use separate exact supplied files but their hardware output is **NOT RUN**. AMD NR is unsupported. This local trial requires the exact currently qualified NVIDIA driver-core file; a different driver core is unavailable until checked. It does not silently load another DLL by basename. The patched RTX 40/20–30 models use a narrowly checked in-memory caller-identity compatibility shim; source DLL bytes are preserved.

The source image must come from a confirmed native UI handoff. Menu/loading and fallback frames without a clean world boundary bypass NR. Resets cover camera discontinuities, off/on, source gaps and resize. A failure with unknown GPU ownership stops rendering and requires restart; the log preserves its reason.

Keep previous TRP and AIO mods for rollback. Installation has a separate mod folder and a backup of the exact profile mod list; do not run two upscaler mods together. Logs contain `[Community NR startup]` and `[Community NR frame]` with requested/effective state, selected profile, source ID, revision and reset/evaluation counts. Runtime files stay local; Git contains implementation and evidence only.
