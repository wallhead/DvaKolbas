# AIO18 Pass 4 runtime breakpoints

Exact module: SkyrimUpscaler.dll SHA-256 `5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81`.

- `SkyrimUpscaler+0x19FA14` — normal SR invocation inside pre-UI handler.
- `SkyrimUpscaler+0x19FCF3` — UI render/depth target bind immediately before UI-phase enable.
- `SkyrimUpscaler+0x19FD00` — UI-phase flag becomes true.
- `SkyrimUpscaler+0x2A95F0` — RSSetViewports wrapper; log incoming/outgoing viewport values.
- Native ID3D11DeviceContext slot 45 / RSSetScissorRects — external breakpoint/vtable trap; compare values against viewport/render/display dimensions because AIO18 does not install a general slot-45 hook.
- `SkyrimUpscaler+0x284EC0` — reshade_begin_effects; capture route flags before they are cleared.
- `SkyrimUpscaler+0x284E70` — reshade_finish_effects; confirm restoration.
- `SkyrimUpscaler+0x284D30` — reshade_present after overlay; confirm pending-final-source consumption.
- `SkyrimUpscaler+0x2A97E0` — ReShade-aware OMSetRenderTargets wrapper; log inEffects, route flags, input RTV resource and replacement RTV.
- `SkyrimUpscaler+0x1A0827` — StatsMenu scene hook clears UI phase.
- `SkyrimUpscaler+0x1A00BF` — StatsMenu post path re-enables UI phase.
- `SkyrimUpscaler+0x299750` — ENB presence probe. Compare call stacks and D3D11 target binds with ENB on/off; do not expect this helper itself to order ENB rendering.

Recommended capture: one normal gameplay frame, inventory/HUD frame, StatsMenu frame, and ReShade-overlay-open frame, first without ENB and then with ENB. Record viewport + scissor pairs, RTV/DSV resources, uiPhase/routeA/inEffects, SR dispatch timestamp, and Present/reshade_present order.
