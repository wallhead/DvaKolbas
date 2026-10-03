# NR branch review and loading failure — 2026-10-03

The [supplied review](../research/nr/reviews/DvaKolbas_NR_Branch_Review_2026-10-03.md) reviews `f20db264`. Its recommendations were checked against the implementation as review evidence. The full NR plan remains **1 of 8** complete. This checkpoint repairs the reversible RTX 4080 SUPER native-SDR Before trial; it does not qualify After FG, other driver cores or other GPU families.

## Loading failure

Both game attempts finished loading the save (`kPostLoadGame`, then Loading Menu closed) but stopped rendering on the first eligible NR world source. Sources 7786 and 8023 reported `NR producer ownership has not retired`; both D3D11 device-removal checks returned zero. Thus the visible loading image was the last delivered image, rather than evidence that save deserialization had hung.

The installed ReShade DLL (SHA256 `b2945c29e7095491a901746b400e58db9b1592ab092bacf2a888ce37f02d08da`) reproduced the failure in an isolated executable: a completed host-created fence reports ReShade's underlying D3D12 device, whereas the queue/resources report the wrapped host. The previous validator conflated this identity mismatch with incomplete GPU ownership. No fence timeout or removal was observed in that reproduction.

The shared Stage now establishes the alternate fence device identity through a private reference fence created by its retained host device. Producer, submission and output-reader fences must belong to that exact host or its verified reference owner. An incoming fence cannot establish an alternate identity. The anchor is also bound to the packet contract device. Foreign anchors, foreign completed fences and pending producers are rejected. Queue/resource identities and real GPU completion checks remain strict. Logs distinguish foreign-device errors from pending producer values (`target`, `completed`).

The original completed-fence rejection was observed RED then GREEN with the actual supplied ReShade DLL. A fresh read-only review identified the public-validator anchor/contract mismatch; its WARP-device regression was also observed RED before fixing the host binding. The isolated full Before host tests real wrapped identities, 240 sources, 239 NR evaluations, live off/on, one resize, unchanged bypass pixels, exact source alpha and explicit reader/runtime retirement. These synthetic tests do not replace the user's next Skyrim loading/visual check.

Clean code `aa3df47ec509` built the Standard NR/SR/FG plugin and passed **159/159 runnable product checks** in 96.98 seconds. The three unavailable Graphics Tools checks (`NativeUIComposition`, `NativeUIBlendState`, `NeuralPeripheralPixels`) were explicitly excluded. Final standalone checks passed **24/24**, without skips, before committing the same code. The [clean ReShade host receipt](../research/nr/runtime-catalog/rtx40-aa3df47-reshade-before.json) and [validation summary](../research/nr/runtime-catalog/reshade-loading-fix-aa3df47.json) retain source/runtime identities, RED/GREEN logs and qualified scope.

The clean plugin DLL and its manifest are now installed in the existing V5.4 NO-LORE trial, with Skyrim absent during replacement and the previous DLL backed up. Hashes confirm the game INI, ImGui INI, MO2 modlist and launch settings were preserved. Corrected Skyrim acceptance remains pending.

## Supplied review findings

### Subsequent FSR device-ownership failure

The next game run loaded the clean `aa3df47ec509` DLL. Save loading completed at 11:04:39; source 16665 evaluated NR successfully at 11:04:40.165. FSR then rejected its command-list/resource device at 11:04:40.174, and the AMD source presenter stopped with `0x80070057`. The fence fix therefore reached NR output, but did not qualify interoperability with the already initialized native AMD presenter. Game device removal remained zero.

An added combined fixture initializes actual FSR SR/FG and manual ReShade first, then lazily evaluates NR before the first temporal dispatch. Using the installed ReShade DLL, the original independent `D3D12CreateDevice` reproduced the exact FSR rejection on source 1. Retaining NR's new ReShade proxy changes the identity exposed by existing native-device children. Independent wrapped NR-only tests had not exercised this native AMD owner boundary.

The game host now supplies NR with its existing FSR bridge device, or its NVIDIA source transport device. BeforeHost retains that exact device, verifies its adapter LUID and removal state before admission, and creates only a separate DIRECT queue. Standalone owners may still create their own device. The game path never requests another device while its presenter is live. FSR's strict ownership check remains unchanged; its failure message now includes the retained host, command-list owner and color-resource owner. NR startup logs the supplied presenter identity. A foreign WARP device is rejected before model initialization.

Both combined routes passed with the installed ReShade: Performance/SRGB and the trial's NativeAA/Gamma22, each with 176 sources, 170 NR evaluations, six NR bypasses, real temporal SR, generation callbacks, UI/foreground preservation, two resized cycles and two suspension/restoration cycles. NR retires before AMD readers on resize and final teardown. These checks qualify synthetic interoperability, not physical display cadence or Skyrim image quality. Fresh read-only review found no blocking issue; absent-presenter late arrival is outside the currently eligible startup routes, exact scene pixels are intentionally unqualified with NR enabled, and After/other GPU/performance work remains open. The corrected Skyrim save-load and visual retest remains required.

| Finding | Checked result and disposition |
| --- | --- |
| Exact NVIDIA driver-core pin and absolute path | Confirmed in `RuntimeOwner.cpp` and staging. Retain for this qualified local trial. A versioned driver-core qualification catalog is needed before a general release; no basename fallback. |
| Synchronous per-source scheduling | Confirmed: producer/readback CPU waits, Stage completion, interop drain and preparation query. This proof bridge needs game measurements before a resource/ticket ring is designed. The loading failure above is a separate identity error, not a measured timeout. |
| Real-source Stage cannot implement true After FG | Confirmed and intentional. Generated packets, unqualified guides and unequal extents remain rejected. Future After requires a separate provider ownership/guide contract; do not weaken Before validation. |
| Duplicate first-use hashing and lazy setup | Confirmed: `Inspect` and `Open` independently lease/hash, then first-use feature/shader initialization waits. Verified-lease transfer, precompiled alpha shader and separated initialization/inference timing remain optimization work. |
| AIO Backbuffer and optional-resource differences | Confirmed absent explicit `Backbuffer`, `ControlMask` and `BidirectionalDistortionField` assignments. This is an unqualified parity difference, not a proven defect. A moving-scene `Backbuffer=Output` A/B and optional parameter contract checks remain pending. |
| ReShade order | Confirmed: source copy → NR Before → ReShade-before → SR → ReShade-after. The fence regression qualifies wrapped-device ownership, not one-effects-execution/depth/color/HUD acceptance for every ordering and ENB combination. |
| Source Gamma22 provisional | Confirmed: explicit decode → linear FP16 NR → encode. ENB-on/off transfer and model suitability still require visual calibration. |
| All live tuning changes reset history | Confirmed in `NvidiaHostNeural.cpp`. Conservative behavior retained until changes can be classified without sacrificing temporal correctness. |
| Independent CI | No tracked GitHub workflow was found. Receipts are local checks; no CI success is claimed. The supplied zero-check count applies to its reviewed head and was not independently queried here. |

Fix the loading blocker and collect native Before gameplay evidence first. Driver portability, pipelining, Backbuffer A/B, color/effects qualification and true After remain separate work, with no completed-milestone increase at this checkpoint.
