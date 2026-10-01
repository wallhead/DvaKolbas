# Proposed runtime trace — AIO18 revision 2

**Not executed here.** All addresses are RVAs in the named module from this package's manifest. Before attaching, verify the loaded module hashes. Use `loaded_base + RVA`; do not paste preferred VAs into an ASLR process. No SkyrimSE.exe RVA is supplied.

## Trace scene/SR/UI first

| Module | RVA | What to capture |
|---|---:|---|
| SkyrimUpscaler | `0x19F420` | Selected pre-UI branch; host flags +343, +487, +489; original target global +47D3B0 |
| SkyrimUpscaler | `0x19F583` | Call stack and actual original engine callee; matching Skyrim executable identity |
| SkyrimUpscaler | `0x19F8FF` | ReShade-before-SR branch; runtime, command list and target views |
| SkyrimUpscaler | `0x19FA14` | SR arguments: proxy index, source/output descriptions and scene content |
| SkyrimUpscaler | `0x19FAD0` | ReShade-after-SR branch; confirm it is not also executed unintentionally elsewhere |
| SkyrimUpscaler | `0x19FCF3` | Post-SR RTV/DSV binding and dimensions; UI phase starts around this region |
| SkyrimUpscaler | `0x2A9B30` | Requested versus forwarded OM targets; phase +E81092; target recognition branch |
| SkyrimUpscaler | `0x2A49E4` | UI CreateTexture2D descriptor and output at host+9E0; compare with scene/output dimensions |

A transition to a different target does not prove every late widget or Scaleform batch uses it. Follow actual draw submission after the menu loop and record viewport/scissor/transform state there.

## Distinguish normal FSR from deferred final-source handling

| Module | RVA | What to capture |
|---|---:|---|
| SkyrimUpscaler | `0x2AB349` | Early preparation, active selector +3DC, mode flags +4E4/+4A5 |
| SkyrimUpscaler | `0x2ABCF5` | Conditions that set pending +E80F9F; ReShade runtime and gate |
| SkyrimUpscaler | `0x284D30` | Actually registered completion callback; match runtime to host+1690 |
| SkyrimUpscaler | `0x284DA2` | Deferred FG source index and full payload |
| SkyrimUpscaler | `0x2AC105` | Missing-callback path clears pending; correlate with logged source absence |
| PDPerfPlugin | `0xEF581` | PrepareV2, camera metadata, reset, dimensions, frame ID |
| PDPerfPlugin | `0xEF719` | Per-frame Configure and current-frame association |
| PDPerfPlugin | `0xEF834` | UI resource/flags and HUD-less handling |
| PDPerfPlugin | `0xEF8C6` | Actual Present-driven interpolation dispatch and function-pointer target |

The other completion-function clone +2AA630 is not the target of the registration proven in this pass.

## Lifetime and stalls

| AMD FG RVA | Capture |
|---:|---|
| `0xBB5D` | Prior composition CPU fence/target; what submission should advance it |
| `0xBB76` | Game-queue wait on prior GPU composition |
| `0xBBA2` | UI duplicate capture; source, destination, flags and generation |
| `0xBCE3` | Next replacement buffer's availabilityFenceValue and replacementBufferFence |
| `0xAB13` | Game-fence drain |
| `0xAB32` | Interpolation-fence drain |
| `0xAB51` | Presentation-fence drain |
| `0xA360` | Replacement destruction, outstanding commands and presenter state |
| `0xBF55` | Underlying ResizeBuffers inputs and HRESULT |
| `0xF4C30` | Wait mode, requested/completed values, callback and producer queue |
| `0xF4050` / `0xF0BD0` | Actual classical versus ML provider execution |

For AMD root-object field offsets, use the recovered selected class: nested presentInfo begins at root+8. Do not confuse a nested field offset with a root-object offset. Do not treat PD's three staging slots, AMD's two interpolation outputs, and the active game backbuffer count as one ring.

For each event record module hash, real frame ID, presentation sequence, buffer index, texture generation/description, queue, fence and target, observed completion, reset reason, and active renderer/backend conditions. Collect successful baselines before testing resize, backend toggle, loading, menus, ReShade changes and device-loss scenarios. Do not force-patch wait loops to return success: that would destroy the resource-lifetime evidence.
