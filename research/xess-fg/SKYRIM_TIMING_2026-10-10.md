# Skyrim XeSS FG timing investigation

## Status

The CPU timing contract and inspected 1.6.1170 call-site wrappers are implemented.
**Actual per-frame Skyrim ordering is not yet qualified.** The installed game
was used for read-only instruction capture; its plugin and INI were not replaced.
Milestone 3 remains partial until the first Intel-host gameplay trace.

## Identified runtime and evidence

Skyrim SE 1.6.1170.0 executable SHA-256:
`c434208894f07f604b852f29b8edc3a58c4de63de783373733e72b2b73f33be9`.
Address Library SHA-256:
`c4093c569a3c83b26587f4b9ea4c55de9ae6e73b84a2af9fb3fbd30e2fe0d452`.
The executable's on-disk `.text` is encrypted (entropy about 8); it cannot supply
valid static instruction witnesses. `Inspect-SkyrimTiming.py` reports this rather
than disassembling encrypted bytes as evidence.

`Capture-SkyrimTiming.py` requested QUERY_INFORMATION and VM_READ only from the
running game. Normal access was denied (Windows error 5); an administrator helper
performed the same read-only operation. It did not inject, pause, patch, or change
settings. The mapped-code scan examined 24.4 MB in bounded chunks, retaining
14,691 bytes of relevant instructions. Each retained function was read twice and
required to match. The compact checked-in receipt is
`engine-call-profile-1170.json`; no full executable memory dump is committed.

## Verified call sites

| Boundary | Caller relocation + offset | Call RVA | Preserved callee / ABI |
|---|---|---|---|
| Before main update | 36550 + 0x11F | 0x641BEF | Main update, relocation 36564; RCX=Main pointer |
| Input sampled | 36564 + 0x567 | 0x646407 | PollInputDevices, relocation 68617; RCX=device manager, XMM1=seconds |
| Simulation end / render start | 36555 + 0x47 | 0x643C47 | Renderer::Begin, relocation 77245; RCX=renderer, EDX=window 0 |

The main update body calls the render routine (36555) at RVA 0x646937, after its
input call at 0x646407. The caller's argument setup, entire E8 call and relative
callee displacement are pinned in the hook profile; all three sites must match
before publishing any patch. Existing detours at the callee entries are retained.
CommonLib's PollInputDevices and Renderer::Begin declarations agree with the
captured register setup. Main update retains RCX in R12 and the loop supplies the
Main singleton; no additional argument is supplied by this call site.

The alternate loop (39039) calls Renderer::Begin at 0x6D220B **before** polling
input at 0x6D2322. It receives no main-source callbacks. A third Renderer::Begin
caller at 0x1195EBC is also excluded. Hooking every input or renderer invocation
would incorrectly create main source cycles in these paths.

The existing jitter hook runs after an inner Renderer::Begin call. It is useful
for source textures but too late for a pre-input XeLL sleep. AIO19's observed
post-Present Sleep/SimulationStart candidate does not prove ordering across
loading/extra Presents; this implementation begins a source only at the verified
main update call instead.

## Admission, lifetime and trace contract

Only a retained Intel host observer activates the new wrappers. Other backends
receive no Intel callbacks. The observer descriptor/context remain allocated
until process exit; Unbind prevents new dispatch but does not reclaim a callback
that another thread already loaded. The timing contract separately checks its
bound thread before accessing phase state or the SDK.

The main update wrapper emits BeforeUpdate before its preserved original call,
and AfterUpdate afterward. The specific main-input wrapper emits InputSampled
after the preserved poll; the main-render wrapper emits BeforeRender before
Renderer::Begin. The host must use one source/epoch-to-32-bit-SDK mapping across
Sleep, all six markers, tags, constants and Present. RenderEnd and Present markers
belong to the actual host submission/Present boundaries, not these input hooks.

For the first trial, record at most 128 initial source cycles with source ID,
epoch, SDK ID, thread ID, boundary and monotonic timestamp. Require
Sleep/SimulationStart → InputSampled → SimulationEnd/RenderStart → RenderEnd →
PresentStart/PresentEnd, with one exact ID and no callback-created loading sources.
Skip/incomplete cycles, thread mismatch, changed runtime/bytes, SDK errors and
extra Presents keep generation inactive with a reason. No fabricated Present-time
markers may repair an unqualified sequence. Other Skyrim versions and actual
menu/save gameplay ordering remain unqualified.

## Verification

The state-machine regression failed before implementation, then passed with
unqualified-bind rejection, genuine pre-input sleep, input/marker ordering,
same-ID mapping, duplicate/extra-Present rejection, thread mismatch and drained
epoch reset. Additional profile tests reject another runtime, changed call opcode,
callee and argument setup; observer tests check unbound/retained-owner dispatch.
Release plugin compilation and `XessFgEngineTiming` pass. These results establish
the implementation contract, not physical Skyrim latency qualification.
