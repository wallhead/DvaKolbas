# Intel host integration checkpoint

Task 9 connects the optional Intel presenter to the existing game-facing host.
Skyrim retains its D3D11 producer and stable render-sized buffer; only the inner
Intel proxy uses native D3D12. Selected SR resources retain their own sizing and
ownership. The initial Skyrim route requires SDR, borderless and Native scale.
Competing AMD/NVIDIA FG and Reflex initialization are excluded on this route.

The inspected 1.6.1170 main-loop calls drive XeLL before input and rendering.
One monotonic SDK ID follows a genuine source through its markers, tags and
Present. Loading/extra/repeated output without matching render timing presents
the real scene and HUD without inventing an ID, tags or latency markers. An
unfinished cycle is abandoned only after native drain at a fresh source boundary.
Incompatible camera/guide conventions hold interpolation off and report why.
The game host requires 128 completed, compatible, ordered temporal sources before
allowing interpolation. This is a runtime admission guard, not a substitute for
the pending gameplay trace/visual qualification.

Validation:

- Full Release build succeeded. The regression sweep completed 211 checks with
  no failures; the FSR ML hardware check was explicitly skipped.
- Twelve focused host/timing/transport/routing/wrapper checks passed. Actual GPU
  tests retain the D3D11 producer identity and native queue, reject foreign
  producer identity, and preserve owners across interruption and fixed-size
  restoration. A production wrapper regression fails without Intel producer
  routing, then passes after restoration. Its boundary facade uses real WARP
  D3D11 device/texture objects; it does not emulate Skyrim or Intel FG.
- Real Intel SDK numerical run on RTX 4080 SUPER: 74/74 eligible sources produced
  two frames; 86 suppressed sources produced at most one; zero SDK errors and
  clean retirement. Three extra no-source Presents produced no generated frame
  and passed scene, opaque HUD and translucent HUD pixel readbacks.
- The new numerical receipt is `host-numerical-2026-10-10.json`; its executable
  and source hashes identify the tested dirty Task 9 build. The earlier visible
  user confirmation belongs to its original build and remains separate.

No Skyrim/MO2 installation, INI, launch settings or published archive was changed
for this checkpoint. Intel gameplay, all SR/NR combinations, UI/telemetry and the
immutable trial package still require their later plan tasks.
