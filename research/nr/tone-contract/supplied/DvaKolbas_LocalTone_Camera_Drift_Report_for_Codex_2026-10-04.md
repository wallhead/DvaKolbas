# DvaKolbas — Local Tone Camera-Drift Investigation Report for Codex

**Target:** `wallhead/DvaKolbas`, branch `codex/nr`  
**Issue:** NR Local Tone causes visible color/brightness drift while rotating the Skyrim camera  
**Goal:** find and fix the integration cause while keeping Local Tone enabled if possible  
**Important:** do **not** treat `LocalTone=0` as the root-cause fix. It is only the currently proven mitigation.

## 1. Executive conclusion

Current evidence points away from frame generation and away from a simple motion-vector sign/scale bug. The strongest facts are:

```text
NR ON + LocalTone > 0 -> visible camera-dependent color/tone change
NR OFF                 -> effect disappears
NR ON + LocalTone = 0  -> effect disappears
```

The existing Skyrim capture measured the RGB change immediately before/after NR, before FSR and FG. Therefore the shift is generated **inside the NR path or its direct input preparation**, not by later presentation.

Most likely integration classes:

1. **NR AutoExposure is active while Dva feeds an already tone-mapped SDR scene.**
2. **Dva does not currently supply the AIO-style `DLSSNR.Backbuffer` reference/fallback used by PureDark's backend.**
3. **The assumed Gamma22 input transfer may not match the actual Skyrim/ENB source domain.**

The next step should be a controlled in-game A/B matrix, not more generic motion-vector changes.

## 2. Existing evidence

Existing research: `research/nr/motion-probe/skyrim-color-5b1eb7e.md`.

Observed:
- NR remained active during the visual shift.
- The effect was reproduced with FG disabled.
- The effect disappeared when NR was disabled.
- RGB changes were already present immediately after NR and before FSR/FG.
- Settled motion vectors were finite.
- Near-still views could still show materially different NR color correction.
- Changing only `NRLocalTone` from `1` to `0` removed the reported effect.

This directly implicates Local Tone, but does not yet tell us whether the cause is intended scene-adaptive model behavior or an incomplete/wrong integration contract.

## 3. Current Dva creation contract

Current direct NR creation writes:

```cpp
DLSS.Feature.Create.Flags = 0x42;
```

For this NGX family, the working interpretation is:

```text
0x40 = AutoExposure
0x02 = MVLowRes
```

So Dva currently requests AutoExposure while the SDR Before path approximately does:

```text
Skyrim/ENB game-format SDR
        ↓
Gamma22 decode
        ↓
linear FP16
        ↓
NR AutoExposure
        ↓
Local Tone
```

This is suspicious because the game/ENB image is already exposure/tone-mapped before Dva decodes it to linear. Camera movement changes scene luminance distribution; NR's own exposure estimate may then move Local Tone around.

## 4. Hypothesis A — AutoExposure is wrong for SDR Before

### Experiment
Create a research-only switch:

```text
Normal: Create.Flags = 0x42
Test:   Create.Flags = 0x02
```

Keep everything else identical:

```text
model
preset/style
intensity
LocalTone=1.0
LocalStructure
motion/depth
SR
FG off
sharpening
input format
```

Use the exact same save/view/camera sweep.

### Interpretation
If `0x42` drifts and `0x02` is stable, introduce an explicit exposure policy rather than leaving a magic flag:

```cpp
NrExposurePolicy::RuntimeAuto
NrExposurePolicy::ProducerManaged
```

Likely SDR Before policy: producer-managed exposure / AutoExposure off. Do not globally disable AutoExposure for future HDR/producer-domain routes without separate qualification.

## 5. Hypothesis B — missing Backbuffer/reference destabilizes Local Tone

AIO RE shows `DLSSNR.Backbuffer` and its subrect are part of the NR contract. The outer AIO payload can leave it null, but PD route logic can select a route/context Backbuffer or fall back internally. Current Dva direct Stage does not supply an equivalent product Backbuffer reference.

### Experiment
Add a research-only per-slot Backbuffer containing the **original pre-NR linear image**:

```text
Prepared linear scene
    ├── DLSSNR.Color
    └── retained Backbuffer reference

NR output
    └── separate UAV
```

Set:

```cpp
p.Set("DLSSNR.Backbuffer", slot.backbuffer.Get());
```

and supply the full Backbuffer subrect.

For the first experiment do **not** alias Backbuffer and Output; use a separate reference to remove read/write ambiguity.

## 6. Four-way Skyrim A/B matrix

Keep all other variables matched:

```text
same save/view/camera sweep
same weather/time
FG off
same SR provider/quality
same runtime
same preset/style/intensity/structure
LocalTone = 1.0
same ReShade/ENB ordering
same sharpening
same cap/VSync
```

| Case | AutoExposure | Backbuffer |
|---|---|---|
| A current | ON | current/null |
| B | OFF | current/null |
| C | ON | pre-NR linear reference |
| D | OFF | pre-NR linear reference |
| Control | any | LocalTone=0 |

Interpretation:
- **B fixes it:** exposure contract is likely the cause.
- **C fixes it:** missing scene reference is likely the cause.
- **Only D fixes it:** both matter.
- **None fixes it:** test transfer function next.

## 7. Hypothesis C — Gamma22 may be the wrong source transfer

If A–D fail, compare:

```text
Gamma22 decode
vs
sRGB decode
```

Keep exposure and Backbuffer fixed during this test. Local Tone reacts to luminance relationships, so a small transfer mismatch can create a much larger adaptive response even when overall color looks approximately correct.

## 8. Do not start by changing motion vectors

Do not initially change:

```text
MVec sign
MVec scale
zero motion
motion units
```

Reason: the synthetic motion probes did not reproduce the effect with otherwise valid pans, near-still Skyrim samples still showed different NR color correction, and `LocalTone=0` removes the defect without changing motion.

Motion should only be revisited if exposure/Backbuffer/transfer experiments fail.

## 9. Improve the Skyrim diagnostic

The old probe compared different views. Add a bounded same-surface temporal diagnostic.

For a tracked building/region:

```text
Frame N output
    ↓ reproject using motion
Frame N+1 same surface
```

Record both input and NR output. Compute:

```text
mean RGB
mean luminance
mean log luminance
median luminance
p10 / p50 / p90 luminance
signed RGB delta
absolute RGB delta
```

Strong root-cause evidence:

```text
input same-surface luminance ≈ stable
NR output same-surface luminance drifts
```

Also record:

```text
LocalTone
AutoExposure effective policy
Backbuffer present/null + identity
source encoding
reset
motion mean
motion invalid/outside count
camera identity
```

Keep capture bounded; no permanent per-frame readback.

## 10. Creation/input logging

At feature creation or route change, log once:

```text
Create.Flags
AutoExposure effective yes/no
source encoding
color format
Backbuffer policy
LocalTone
work extent
runtime SHA
```

Do not invent an `ExposureTexture` product path unless the exact NR runtime contract is proven to consume it. AIO's recovered mapped NR writer did not expose a separate exposure texture.

## 11. Parameter-range cleanup

AIO19 chain validation constrains four per-pass tuning floats to `[0,1]`, while Dva's Build14 sanitizer permits strengths up to `2.0`. The current defect was also seen around LocalTone=1, so this is probably not the root cause, but >1 values complicate diagnosis.

For all experiments force:

```text
LocalTone = 1.0
Intensity = matched known value
LocalStructure = matched known value
```

Do not change product clamping until RE confirms which four chain floats map to which controls.

## 12. Fallback if NVIDIA Local Tone itself is unsuitable

If all integration A/Bs fail but LocalTone=0 is consistently stable, treat NVIDIA Local Tone as unsuitable for this Skyrim SDR route.

Possible shipping fallback:

```text
NR LocalTone = 0
        ↓
stable NR result
        ↓
optional deterministic TRP local-tone shader
```

A deterministic current-frame shader could use low-frequency log luminance and bounded local contrast while preserving chroma. Do **not** build this until AutoExposure, Backbuffer and transfer tests are complete.

## 13. Codex implementation order

### Phase 1 — bounded experimental controls
Add research-only switches for:

```text
AutoExposure ON/OFF
Backbuffer OFF/reference
```

No shipping default change.

### Phase 2 — Skyrim A/B
Run A/B/C/D + LocalTone0 control with FG off and bounded same-surface diagnostics.

### Phase 3 — root-cause implementation
- Exposure wins → explicit SDR producer-managed exposure policy.
- Backbuffer wins → promote retained Backbuffer into `ImagePacket` / Stage slot / validation / states / lifetime.
- Both win → implement one explicit SDR input contract containing both.
- Neither wins → Gamma22 vs sRGB.

### Phase 4 — regression coverage
Cover:

```text
creation flags
Backbuffer device identity
Backbuffer state/subrect/lifetime
foreign Backbuffer rejection
resize/recreate
off/on/reset
ReShade proxy identity
device loss
three retained slots
alpha
FSR PreparedLinear
```

### Phase 5 — real game acceptance
Re-test:

```text
FSR PreparedLinear
DLAA
FG off/on
ReShade-before/after
resize
interior/exterior
bright sky entering/leaving frame
```

## 14. Do not change yet

Do not simultaneously change:

```text
motion sign/scale
history reset policy
depth
slot count
NR runtime DLL
alpha restore
NR/FSR queue topology
LocalStructure
```

Those changes would confound the Local Tone experiment.

## 15. Root-cause ranking

Current working ranking:

```text
1. AutoExposure + already tone-mapped SDR mismatch       HIGH
2. missing/incorrect Backbuffer scene reference          MEDIUM-HIGH
3. Gamma22 vs real source transfer mismatch              MEDIUM
4. Local Tone model behavior inherently scene-adaptive   MEDIUM
5. motion-vector integration bug                         LOW-MEDIUM
6. FSR/FG causing the color drift                        LOW
```

This is a hypothesis ranking, not proof.

## 16. Definition of success

The preferred fix keeps:

```text
LocalTone = 1
```

and makes stationary world surfaces remain visually stable while the camera moves.

Acceptance requires:

```text
no visible building/material brightness pumping
no color drift on camera rotation
NR stays active
no new ghosting
no motion regression
no alpha/HUD regression
no reset/off-on regression
no new reader/device lifetime failure
```

Quantitatively, same-surface NR-output temporal luminance should approximately track same-surface source temporal luminance rather than adding a large model-induced component.

## 17. Recommended immediate Codex task

Implement only this bounded experiment first:

```text
A) 0x42, no Backbuffer reference
B) 0x02, no Backbuffer reference
C) 0x42, pre-NR linear Backbuffer reference
D) 0x02, pre-NR linear Backbuffer reference
```

Expose it only in a research build/configuration. Add one-time creation logging and bounded same-surface diagnostics. Do not alter product defaults until the Skyrim A/B identifies which variable removes the drift.

Until then, the safest user-facing mitigation remains:

```text
LocalTone = 0
```
