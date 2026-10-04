# AIO19 native preparation and passive texture checkpoint

2026-10-04. This is research instrumentation, not a production tone-drift fix.
No installed DLL, INI, MO2 launch setting or profile was edited. The completed
NR milestone count remains **1 of 8**.

## Ghidra and Capstone

A fresh Ghidra 12.1.3 analysis imported the exact **AIO19** PDPerfPlugin image,
SHA256 `ff6c58391501006474e36afaac2b5a5cfceda47850abdbbcc7f2fe5521004435`.
Earlier AIO18 analyses were not substituted for it. The headless run exited 0;
ten requested targets were decompiled (target `A6670` belongs to `A65F0`).
Capstone independently checked the relevant selector/copy instructions.

The local Ghidra database is
`C:/Users/user/Documents/ChatGPT/RE_Projects/DvaKolbas_NR_AIO19/AioNR19PD.gpr`.
The proprietary imported image/database and full decompile remain local.
[The receipt](native-preparation-receipt.json) pins the analysis outputs and
owned fixture. Reopen the database to inspect the following recovered routes:

| PD RVA | Evidence |
| --- | --- |
| `A5FD0` | Caches the `0x598` request at backend `+3240`; copies packet `+8` into an internal frame, with later wrapper overrides. |
| `A62E0` | Caches the normalized internal frame at backend `+3C80` before dispatch. |
| `A0A70` | Canonicalizes input scale: nonpositive or at least 1 becomes 1; smaller positive values clamp to at least 0.25. |
| `A0AA0` | Method 2 remains 2; otherwise the selector returns whether canonical scale is below 1. |
| `A1E00` | Uses input `GetDesc`; method 0 assigns the input texture to working color slots and returns before the preparation shader path. |
| `A2740` | With the preparation flag clear, returns after a conditional compatible-resource copy, without entering its resolve shader branch. |

Offline decoding of the two prior captures (30 retained requests total) finds
requested method 0, scale 0 (canonical 1), no alternate color request, and
Color/Output pointer aliasing. Requested motion scale is 2560/1440; that does
**not** establish guide texture dimensions. The static selector consequently
predicts native preparation method 0. These are cached requests and conditional
code paths, **not observed GPU shader execution or final vendor frames**.

AIO's saved `mDLSSNRBeforeUpscaling=false` also differs from Dva's implemented
Before path. That setting alone does not establish executed placement or the
cause of drift. Preserve full Tone 1 and distinct styles; do not ship a format
or placement patch based only on this static evidence.

## Passive metadata qualification

[NativeTextureMetadata.py](NativeTextureMetadata.py) reads seven fields
corresponding to native Texture2D `GetDesc`: width, height, mip count, array size,
DXGI resource format, sample count and sample quality. It makes no target COM
call and creates no target threads or GPU work. It requires the physically
mapped Windows d3d11 image and the exact getter address/code hash. Unknown
proxy implementations are unsupported; changing fields/interfaces are refused.

The getter was recovered with Capstone and checked against real `GetDesc`
calls on **owned** hardware textures. [The owned receipt](native-texture-validation.json)
records **11 matching descriptions and 5 refusal cases**: ordinary/FP16/float
formats, typeless depth formats, multiple mips/array slices, MSAA and staging;
unknown module/getter, changed code/interface and changing fields are rejected.
The fixture never calls COM on Skyrim objects. Run only while Skyrim is closed:

```powershell
C:/Python314/python.exe research/nr/tone-contract/runtime-observer/TestNativeTextureMetadata.py --output out/native-texture-validation.json
```

The integration brackets metadata reads with another retained texture-pointer
set/backend/chain check. Matching reads still do not prove an atomic snapshot,
object lifetime, GPU ownership or request freshness. No views, transfer function,
pixels, active viewport or complete descriptor flags are captured.

## Next game gate

Select only AIO19 in V5.4 NO-LORE; start normally, load the same building scene,
enable NR Style 0 / Tone 1 and keep FG off. Run `Capture-AIO19-NR.cmd` as
administrator and rotate around the building for about 15 seconds.
`Capture-AioRequest.ps1` verifies binary/profile admission, captures selected
settings and before/after hashes, and writes a unique local output directory.
It does not edit settings. Unknown texture getters remain explicitly unsupported.
Wrapper admission concerns the selected disk files/profile, not independent
proof of the running MO2 profile or mapped host DLL. The Python reader separately
qualifies physically mapped PD/NR images and selected route instructions.

Actual Skyrim texture formats have **not yet** been captured with this reader.
They are the next evidence needed before choosing a production color-contract
change. The wrapper refuses the currently selected Dva profile rather than
changing it. The old debugger entry remains blocked by the unqualified attach
hook; its admission checks were not weakened.
