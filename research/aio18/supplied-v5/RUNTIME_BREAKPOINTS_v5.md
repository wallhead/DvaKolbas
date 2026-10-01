# AIO18 FSR RE — runtime breakpoint plan v5

All offsets are module RVAs for the exact AIO18 Hotfix1 hashes in `AIO18_FSR_RE.md`. Live address = loaded module base + RVA. These are proposed runtime experiments; no game/GPU run is claimed by the static package.

## Goal 1 — prove the Community Shaders temporal-guide chronology

| Module RVA | Observe |
|---|---|
| `SkyrimUpscaler +0x19DCC0` | Entry to the CS live-postprocessing bridge. Record caller stack and CS version/module identity. |
| `SkyrimUpscaler +0x19DD14` / `+0x19DDC4` | Which branch calls `PrepareCSUpscaleInput +0x2654B0`. |
| `SkyrimUpscaler +0x2654B0` | Record host render dimensions `+0x34C/+0x350`, raw depth/MV `+0x7D0/+0x828`, and return value. |
| `SkyrimUpscaler +0x2656F5` | Call to `CaptureCSDepthAndMotion(true)`. Record source texture identity/desc before and prepared identities after return. |
| `SkyrimUpscaler +0x2A1D74` / `+0x2A20E5` | Shader-transform helper calls. Capture source/destination identities, viewport, constant-buffer values and format. |

Per hit, record:
- texture pointer, width, height, format, array/mip/sample counts;
- host `+0x1618/+0x161C` capture-scale values;
- current jitter host `+0x08/+0x0C`;
- reversed-depth host `+0x2FC`, near/far `+0x2F8/+0x2F4`;
- whether CS-specific capture succeeded or generic resources were used instead.

## Goal 2 — prove the same prepared guides feed SR and FG

| Module RVA | Observe |
|---|---|
| `SkyrimUpscaler +0x294CF5` | SR depth = host `+0xB50`. |
| `SkyrimUpscaler +0x294D03` | SR motion = host `+0xBA8`. |
| `SkyrimUpscaler +0x294FBD` | Final SR common payload immediately before `EvaluateUpscaler`. |
| `SkyrimUpscaler +0x2944D9` | FG depth = host `+0xB50`, unless depth suppression is active. |
| `SkyrimUpscaler +0x2944E7` | FG motion = host `+0xBA8`. |
| `PDPerfPlugin +0xEF581` | FSR FG PrepareV2 dispatch; compare depth/MV identity and dimensions with host values. |

Run both with Community Shaders enabled and with its specialized bridge disabled/unavailable if a controlled configuration permits it.

## Goal 3 — reactive/T&C and exposure policy

At `SkyrimUpscaler +0x294FBD`, dump the common payload and verify payload `+0x20` is null on the normal SR producer path.

For DX11 FSR:
- `PDPerfPlugin +0xFF0FC`: reactive resource read from payload `+0x20`.
- `PDPerfPlugin +0xFF096`: null external exposure conversion.
- `PDPerfPlugin +0xFF2DD`: null transparency/composition conversion.
- `PDPerfPlugin +0xFF475`: actual FSR3 dispatch.

Record whether any alternate producer supplies non-null masks. Test visually stressful scenes: particles/fire, water, alpha vegetation, magic effects, animated textures and transparencies.

## Goal 4 — render-size changes versus context flags

Context-create landmarks:
- `PDPerfPlugin +0xFE3BC`: DX11 base flags `0x220`.
- `PDPerfPlugin +0xFE3CD`: inverted-depth alternative `0x228`.

Record context creation count/lifetime and host render dimensions while changing dynamic/render scale if the configuration permits it. Determine whether dimensions actually vary without SR context recreation. This is required before judging the absent bit-6 dynamic-resolution flag.

## Goal 5 — reset coverage, not merely reset capability

Watch:
- host SR reset byte `SkyrimUpscaler-object +0x264`;
- PD FSR-FG backend resetPending `+0x1C9`;
- depth-convention change latch host `+0x2FE`;
- prepared resource identities `+0xB50/+0xBA8`.

Trigger one event at a time:
1. camera cut / forced teleport;
2. loading door / worldspace load;
3. StatsMenu embedded 3D scene;
4. resolution/resize/alt-tab transition;
5. backend enable/disable or SR/FG mode change;
6. Community Shaders bridge activation/deactivation where reproducible.

For each event, log who writes each reset flag, whether SR/FG consumes it on the first eligible frame, and when it clears.

## Minimal success criteria for the runtime pass

A useful capture should establish, with call stacks and resource descriptors:
1. one full CS frame from live-postprocessing hook through prepared guide creation;
2. identity equality/relationship of those prepared guides at both SR and FG dispatch;
3. actual reactive/exposure/T&C values at dispatch;
4. actual provider/backend and context flags;
5. at least one temporal reset event from producer through consumption/acknowledgment.
