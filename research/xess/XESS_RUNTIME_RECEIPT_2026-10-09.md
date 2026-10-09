# XeSS runtime and initialization receipt — 2026-10-09

Milestone 2: runtime loading, real adapter routing, sizing and initialization verified. Temporal execution, colour reconstruction and Skyrim integration are not qualified yet.

Official SDK bundle 3.0.2, commit `8fe81bdbbaf00b3c1b733fd0d830c333dc84e6f0`. D3D12 SR dispatcher SHA256 `251659dd84a3e84de67c886a4186e01f3eca49b00641906fe38bb6b807e5d5b7`, 77,795,704 bytes. Dispatcher reports **SR API 2.0.2**. AIO19's byte-identical runtime was extracted and hash-verified; only the two D3D12 SR headers, distribution license and one runtime were acquired. No AIO19 caller was executed.

## Actual GPU witness

`TRPXessRuntimeProbe --plugin-directory <absolute minimal-sdk/SKSE/Plugins>` ran on **NVIDIA GeForce RTX 4080 SUPER**, vendor `10de`, device `2702`. D3D11 and D3D12 LUIDs matched (`00000000:c044d0a6` for this session). The compute adapter was selected from the actual D3D11 device LUID. No Streamline interposer or NGX loader was loaded.

For each mode: CreateContext, logging registration, Intel implementation query, driver check, sizing, blocking pipeline build, Init, GetInitParams and DestroyContext returned success (0). Intel implementation was 0.0.0, the SDK's documented non-Intel result; it does not identify a separate cross-vendor implementation version.

| Mode | Output | Optimal input | SDK minimum | SDK maximum | Warm build + init |
|---|---|---|---|---|---|
| Native / AA | 1280×720 | 1280×720 | 1280×720 | 1280×720 | 22.71 ms |
| Quality | 1280×720 | 753×424 | 753×424 | 1280×720 | 18.74 ms |
| Performance | 1280×720 | 557×314 | 557×314 | 1280×720 | 18.32 ms |

These dimensions are actual SDK queries, not fixed FSR scale factors. Timings are initialization observations, not frame-performance measurements. The first cold Native initialization observed 445.06 ms.

## Effective LDR flag finding

Requesting `XESS_INIT_FLAG_LDR_INPUT_COLOR` (64) succeeded, but GetInitParams reported flags 0. Capstone inspection of the hash-pinned official DLL establishes the cause: exported D3D12 Init jumps to RVA `0x1a69c0`; at `0x1a6a30` it tests bit `0x40`, then `xor eax, 0x40` at `0x1a6a34` and writes the cleared flags into the copied init structure at `0x1a6a37`, before calling the implementation. This is a dispatcher-side transformation, not evidence that our loader failed.

The initialization probe accepts and warns about this **specific** 2.0.2 readback difference. Other readback differences still fail. We do not claim that tonemapping is disabled. Task 4 must execute actual frames and verify colour roundtrip/convergence before gameplay integration; the design has been corrected accordingly. No SDK patch was applied.

## Verification and limits

- Loader regression failed first on MissingRuntime, then passed with implementation. Adapter mismatch likewise failed before the LUID validation and passed afterward.
- Runtime checks: absent file, relative root, x86 PE, missing required export, partial-load cleanup, Unicode directory, foreign loaded basename and shared-owner module retention.
- Acquisition rejected tampered DLL bytes and accepted an idempotent reacquisition of the pinned payload.
- Focused compatibility suite: XeSS runtime plus existing FSR runtime, backend selection, GPU matrix, compiler policy and public INI schema — **6/6 passed**.
- Every created probe context was explicitly destroyed successfully. This probe submits no temporal frames; SDK Init manages its internal uploads.

AMD and Intel execution remain unqualified. Installed mods, INIs and MO2 profiles were not changed.
