# NR branch review and loading failure — 2026-10-03

The [supplied review](../research/nr/reviews/DvaKolbas_NR_Branch_Review_2026-10-03.md) reviews `f20db264`. Its recommendations were checked against the implementation as review evidence. The full NR plan remains **1 of 8** complete. This checkpoint repairs the reversible RTX 4080 SUPER native-SDR Before trial; it does not qualify After FG, other driver cores or other GPU families.

## Loading failure

Both game attempts finished loading the save (`kPostLoadGame`, then Loading Menu closed) but stopped rendering on the first eligible NR world source. Sources 7786 and 8023 reported `NR producer ownership has not retired`; both D3D11 device-removal checks returned zero. Thus the visible loading image was the last delivered image, rather than evidence that save deserialization had hung.

The installed ReShade DLL (SHA256 `b2945c29e7095491a901746b400e58db9b1592ab092bacf2a888ce37f02d08da`) reproduced the failure in an isolated executable: a completed host-created fence reports ReShade's underlying D3D12 device, whereas the queue/resources report the wrapped host. The previous validator conflated this identity mismatch with incomplete GPU ownership. No fence timeout or removal was observed in that reproduction.

The shared Stage now establishes the alternate fence device identity through a private reference fence created by its retained host device. Producer, submission and output-reader fences must belong to that exact host or its verified reference owner. An incoming fence cannot establish an alternate identity. The anchor is also bound to the packet contract device. Foreign anchors, foreign completed fences and pending producers are rejected. Queue/resource identities and real GPU completion checks remain strict. Logs distinguish foreign-device errors from pending producer values (`target`, `completed`).

The original completed-fence rejection was observed RED then GREEN with the actual supplied ReShade DLL. A fresh read-only review identified the public-validator anchor/contract mismatch; its WARP-device regression was also observed RED before fixing the host binding. The isolated full Before host tests real wrapped identities, 240 sources, 239 NR evaluations, live off/on, one resize, unchanged bypass pixels, exact source alpha and explicit reader/runtime retirement. These synthetic tests do not replace the user's next Skyrim loading/visual check.

## Supplied review findings

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
