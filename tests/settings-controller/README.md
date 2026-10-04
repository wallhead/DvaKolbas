# Production settings controller fixture

`RendererSettingsController.cpp` is copied byte for byte to the build directory
so its quoted owner includes resolve to the explicit facades here. The controller,
draft validation, source generation requests and requested/effective upscaler
configuration use production code. `SourceUpscalerConfiguration.cpp` is also
copied unchanged: request adoption and deferred host reconfiguration are actual
production methods. Other game owners, texture descriptors, persistence, weather
service and vendor operations are recording facades. No product conditional is
introduced.

`RendererSettingsController` checks session Apply, Save, failed persistence,
invalid drafts, NR/FG toggles, provider restart staging, rejected scaled After,
external provider failure and a terminal NR owner. It also checks that community
NR cannot enable the separate legacy NR pass.

`NrControllerApplyGpu` and `NrControllerApplyReShadeGpu` call that same controller
through 240 actual native SDR NR sources. Five Apply boundaries begin with a
genuinely pending encoder reader: tone, NR disable, style, After to Before and
Before to After. Settings reach the source snapshot; Apply does not drain the
GPU on the settings thread. Destructive changes retire old work when the source
owner consumes them. These cases use the unchanged shared NR host and actual
ReShade wrapper respectively. Alpha, bypass pixels, history reset and one-pass
counts retain independent checks.

`--omit-controller` is a deliberate negative on the controller GPU executable:
the underlying source tests still pass, but the five-Apply coverage check fails.
The ordinary host executable rejects this flag.

`SourceUpscalerDeferred` checks UI/early-source deferral, retire/create/resume
ordering, coherent effective settings/history, three terminal failure points,
no retry, restart staging and FSR dispatch-only sharpness updates. Its
`--omit-deferred` negative fails the actual orchestration oracle.

`SourceUpscalerDeferredGpu` supplies a genuine pending GPU copy to the retirement
boundary. The unchanged deferred host waits for its completion before feature
creation checks all 4,096 old-source bytes. UI deferral leaves that copy pending.
`--omit-wait` deliberately fails the completion/pixel oracle. Submitted resources
are retained by the existing single-shot probe guard until independent completion
is confirmed; uncertainty quarantines the entire capsule. This is an independent
GPU reader, not an opaque NVIDIA worker or actual vendor feature recreation.

This does **not** exercise mouse dispatch in the End menu, complete NvidiaHost
Present dispatch, opaque vendor FG readers, live presenter replacement,
generated pixels or Skyrim. The GPU fence covers actual NR preparation/delivery.
Provider/presenter changes remain restart-staged; live NR toggles and placement
remain separate requirements.
