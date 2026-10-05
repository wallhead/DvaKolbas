# Standard ImGui and sequential NR passes

Approved change: replace the custom overlay with stock dark ImGui and exactly
three tabs: **DLSS**, **NR**, **Frame generation**. The DLSS tab also contains
FSR controls. Preserve the frame-time chart, clickable rendering graph, input
capture, live shortcuts and the shared Apply / Save as default / Discard actions.

The menu uses one scrolling settings column. Status and advanced controls are
collapsible; native UI settings are under Frame generation and rendering,
ReShade, measurements and diagnostics are under DLSS. Appearance presets use
ordinary Selectable rows and RadioButtons. Pipeline navigation uses stable IDs
even while runtime status text changes. The graph displays native UI joining
the final output after generation.

## Neural Rendering

Community NR supports **one to three sequential native SDR passes** on either
existing placement: Before upscaling, or After upscaling before FG. Each pass
has its own style, intensity, tone, structure, skin structure and automatic skin
mask. Passes 2 and 3 can follow Pass 1; relinking preserves their independently
saved overrides. After still requires DLAA or FSR Native AA; reduced guides,
HDR and reconstruction model changes remain unqualified.

One Stage owns the process allocator callbacks and one independent vendor
feature per active pass. Each retained frame slot owns its parameters,
descriptor heaps and intermediate output textures. All passes record in one
command list and retain one ticket through vendor, bridge and downstream reader
retirement. Styles drain pending readers before changing; pass-count changes
retire and recreate preparation. Partial vendor creation or recording failures
retain uncertain owners. A third-evaluation fault regression covers failure
after two successful vendor evaluations. Default one-pass allocations remain
one pass. The legacy NR path remains limited to two; enabled legacy requests
above two are rejected at startup and Apply instead of silently clamped.

**Stable colors is removed** from preferences, controls and rendering. Old
StableColors / NRStableColors keys are ignored and removed on save/conversion.
Its shader is not linked into the product; standalone historical utility tests
remain available. Pass 3 persists in `[NR PASS 3]` with the same inheritance
rules as Pass 2. Existing INIs retain one pass and all other choices.

## Verification

- Test-first failures demonstrated unsupported third-pass serialization,
  missing legacy limits, the old split layout, pass-count repair resetting to
  one, copy delivery incorrectly requiring an RTV, and conversion retaining
  the removed color key. The corresponding checks passed after implementation.
- Release plugin and all configured test targets built successfully. Stale
  MSVC incremental LTCG artifacts caused C1001 in two small test executables;
  targeted clean rebuilds passed unchanged source. No optimization was disabled.
- **229 selected noninteractive checks passed**, including **63 GPU** and
  **seven ReShade** checks, with no selected failures or skips. Local logs and
  JUnit are under `out/research/plain-imgui-three-pass-*`.
- Real two/three-pass GPU tests inspect separate features, input-to-output
  chaining, all tuning parameters, finite changed RGB and exact alpha over
  repeated frames, per-pass style changes and NR off/on. A deliberate mutation
  that recorded only one pass produced three expected failures; restoring the
  chain returned the Stage checks to green.
- Host/controller GPU tests change 1 -> 3 -> 2 -> 1 passes and Before/After
  placement with genuinely pending readers. The ReShade controller cycle
  passed 240 sources and five gated Apply transitions on the wrapped device.
- Headless ImGui checks cover stock palette and geometry, NR tab presence,
  navigation at 780/1400 pixels, stable IDs during changing status, balanced
  stacks, linked/independent pass preservation and persisted window geometry.
- Independent static review found no confirmed P1/P2 defects.

Three existing debug-device checks were rerun separately and remain environment
failures: **NativeUIComposition** (`WARP debug device`), **NativeUIBlendState**
(`device`) and **NeuralPeripheralPixels** (`debug interface 0x887A002D`). They
are not counted as passing. Interactive NVIDIA foreground-window tests were
not rerun. The user previously authorized continuing without Graphics Tools.

## Trial and limits

The staged trial preserves DLAA -> NR After -> NVIDIA FG, Style 0 / Tone 1,
one pass and all other saved values, removing only the retired Stable colors
setting and updating pass-count comments. MO2 launch/profile settings and
runtime payloads are preserved. Back up DLL, INI and manifest before installing.

New-build Skyrim testing is pending. Verify End-menu Apply, 1 -> 2 -> 3 -> 1
passes, independent pass styles, NR/FG off/on, camera colors and HUD. Synthetic
GPU tests demonstrate execution and ownership; they do not establish Skyrim
visual quality or qualify additional GPU families, model binaries or drivers.
