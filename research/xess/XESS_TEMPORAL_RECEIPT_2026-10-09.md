# XeSS standalone temporal receipt — 2026-10-09

Milestone 4 implementation and numerical validation are ready. **Human visual qualification is pending.** XeSS is not connected to Skyrim yet; no installed mod or MO2 files were changed.

## Runtime and ownership

The pinned official SDK 3.0.2 dispatcher reports SR API 2.0.2. SHA256: `251659dd84a3e84de67c886a4186e01f3eca49b00641906fe38bb6b807e5d5b7`. The actual RTX 4080 SUPER's D3D11 adapter supplies the matching D3D12 device. Input conversion, shared guides/output, SDK context, command queue, runtime file lease and module remain owned through retirement.

The context requests LDR input, exposure scale 1, no external exposure texture and no auto-exposure flag. As recorded in the runtime receipt, this official dispatcher removes the requested LDR bit. The owner accepts only that exact SR 2.0.2 readback difference. The tests establish color behavior for these scenes; they do not establish that vendor tonemapping is disabled for every possible image.

`ConfigureGuides` runs after SDK sizing and before the first execution. The synthetic producer explicitly supplies undilated current-to-previous UV motion, converted by the SDK velocity scales to input pixels. This does not qualify unknown game shader guide contents.

## Numerical scene

`XessTemporalGpu` executed 480 real SDK frames: 80 each for Gamma22/SRGB × Native/Quality/Performance, output 640×360. SDK input sizes were 640×360, 377×212 and 279×157. Each case contains uniform colors, an epoch/reset comparison, stationary checker edges under subpixel jitter and an analytic camera pan with a foreground plane.

- Decoded constant-color means matched independently calculated Gamma22/SRGB values within the 0.02 tolerance. Re-encoded RGB bytes reproduced 128/64/192 within three byte steps.
- Reset after a source epoch change reproduced the independent initial uniform frame within 0.002 per channel.
- Late stationary-edge frame differences had RGB RMS 0.002471–0.003741, below 0.02. This measures settling; it cannot certify subjective edge shimmer.
- Maximum foreground centroid alignment error was 0.5000 display pixels, below the three-pixel test limit. All readbacks were finite and bounded.

The visible run `out/validation/xess/visible-20261009-105908-9434a4.log` completed 960 actual SDK frames at 1280×720, Gamma22 only, through Native → Quality → Performance. Queried sizes were 1280×720, 753×424 and 557×314. Its largest centroid error was 0.5765 pixels; static RMS was at most 0.002187. The window closed after successful cleanup. A human still needs to confirm convergence, camera stability and acceptable shimmer.

## Failure and retirement checks

Lifecycle tests use a separate explicitly selected SDK boundary fixture and real WARP D3D12 fences. They cover partial initialization cleanup, exactly-once successful destruction, poisoned-context refusal, failed destruction retry and pending-reader retention. These fixture calls are not included in the official SDK frame counts.

The real GPU suite also checks:

- Rejecting duplicate source IDs, changed extents and same-sized replacement resource identities before SDK dispatch.
- Rejecting producer writes after bridge ownership commits.
- An actual downstream `CopyResource` held on another queue: retirement refuses while its completion fence is pending, retains context/module/output, and succeeds after completion.
- Allocator reuse waits for a deliberately held real queue submission: measured 66.81 ms in the numerical run and 60.43 ms in the visible run.
- Forced vendor Execute failure leaves unsubmitted work retained; explicit discard and subsequent drain permit cleanup. Partial failed vendor work is never submitted.

## Build checks and limits

The plugin and standalone targets built with the shared neutral SDR converter. `FsrColorConverter` remains a compatibility alias; existing FSR color/runtime/frame tests pass. Five XeSS checks also passed with FSR and FSR FG disabled, after which the integrated build configuration was restored.

The focused regression group comprises 14 checks: BackendSelection, RendererGpuMatrix, PluginCompilerPolicy, FsrRuntime, FsrParameters, FsrLifecycle, FsrColorContract, FsrFrameIntegration, XessRuntime, XessFrameAdapter, XessLifecycle, XessTemporalGpu, XessRuntimeGpu and PublicIniSchema.

D3D11/D3D12 debug layers are unavailable and explicitly reported as skipped. No debug-layer qualification is claimed. Actual AMD/Intel hardware, gameplay, NR, FG and HDR remain unqualified for XeSS. Intel XeSS FG/XeLL remains deferred.

Rerun the visible check with `tools/xess/Start-Xess-Visible.cmd`, keeping Skyrim closed. It writes a unique log and changes no Skyrim/MO2 settings.
