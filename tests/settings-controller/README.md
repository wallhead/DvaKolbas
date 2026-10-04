# Production settings controller fixture

`RendererSettingsController.cpp` is copied byte for byte to the build directory
so its quoted owner includes resolve to the explicit facades here. The controller,
draft validation, source generation requests and requested/effective upscaler
configuration use production code. Game owners, persistence, weather service and
the vendor backend are recording facades. No product conditional is introduced.

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

This does **not** exercise mouse dispatch in the End menu, deferred production
NvidiaHost reconfiguration, opaque vendor FG readers, live presenter replacement,
generated pixels or Skyrim. The GPU fence covers actual NR preparation/delivery.
Provider/presenter changes remain restart-staged; live NR toggles and placement
remain separate requirements.
