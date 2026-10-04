# NR implementation progress — 2026-10-03

**3 of 8 milestones complete:** native-size post-SR contract (Task1), inherited runtime/catalog (Task2), shared Before/After source stage (Task3).

**Latest checkpoint, 2026-10-04:** the [source-stage evidence](../research/nr/post-sr/SOURCE_STAGE.md) qualifies Native AA/DLAA After. The stage order is `upscale -> source effects -> NR -> optional FG -> UI`. The reduced-guide experiment fails thin/unaligned surfaces, so scaled After remains unavailable. Actual FSR Native AA/NR/upload passed 55,296,000 enhanced FG-source bytes; native/ReShade stage switching with a genuine pending encoder and resize passed. Live End controls expose Before/After upscaling (before FG). Actual NVIDIA vendor chaining, combined FSR generated output, remaining lifecycle/performance and Skyrim acceptance remain open. The native After trial is now installed in V5.4 NO-LORE with a backup of the accepted Before DLL/INI. Only the DLL and placement changed; DLAA, NVIDIA FG, full tone and MO2 settings are preserved. The clean-source build matrix and 196 passing checks are recorded in the [install receipt](../research/nr/post-sr/clean-build-install.json); three unavailable Graphics Tools checks were excluded. The [first running-game receipt](../research/nr/post-sr/skyrim-dlaa-after-game-result.json) confirms After NR processing, NVIDIA x2 output while After is selected, live Before/After switching and NR off/on without error/critical log entries; the owner reports it seems working. Latest logged placement is Before. Broader scene/lifecycle/performance acceptance remains open.

**Historical post-FG research checkpoint, 2026-10-04:** resumed true After-FG feasibility.
The [independent source-pair guide experiment](../research/nr/post-fg/GUIDE_RECONSTRUCTION.md)
passes 26 research assertions, including explicit negative evidence: subpixel
boundaries and a surface hidden at both endpoints have incorrect intermediate
guides. Full coverage is insufficient. Official pinned FSR source contains
intermediate depth and packed motion candidates, but their exact-runtime access,
alias lifetime and correspondence to generated color remain unqualified.
No product/INI/MO2 changes or GPU execution occurred. Task 1 stays open; **1 of 8**.

**Latest tone checkpoint, 2026-10-04:** the owner reports the SDR-byte camera
fix looks stable, followed by “seems fine” after the style/FG check. The
[follow-up running-game receipt](../research/nr/tone-contract/sdr-byte-trial-fg-game-result.json)
confirms clean `5d9a3b873b72`, full Tone 1, Stable colors off, live styles 0/1,
NR off/on and successful FSR FG generation/resumption. The owner also reports
DLAA/DLSS work; [NVIDIA-route prefixes](../research/nr/tone-contract/sdr-byte-trial-dlss-game-result.json)
confirm active full-tone NR and x2 runtime output on RTX 4080 SUPER. This supports
local Before processing/scene acceptance. Broader scene/hardware acceptance remains open; true After-FG NR is still incomplete. **1 of 8** remains complete.

The earlier Dva game check reports that apparently only Style 0 shifts. A captured prefix confirms 11,400 NR evaluations, successful live style changes, full tone and no error/critical logs; a longer Style-0 interval has unchanged sparse reset counts. At that checkpoint AIO's active Style 0 had not yet been tested. Nonzero styles are not yet fully qualified, and the count remains **1 of 8**.

That reference test is now complete: **AIO Style 0 / Tone 1 has no shifting**, and the user confirms visible NR off/on differences with stable camera rotation. Its startup/post-session configurations, native feature creation and exact binary pins are [retained](../research/nr/tone-contract/aio-style0-game-result.json). AIO ended with NR requested off; all other parsed controls remain unchanged. Prioritize actual AIO/Dva input/output, viewport, guide and effective-control evidence before changing the production color contract. Linear-FP16 conversion is a hypothesis, not an established root cause; full tone and distinct styles remain required. **1 of 8** remains complete.

The [external effective-control observer](../research/nr/tone-contract/runtime-observer/README.md) now has owned CPU lifecycle checks and a two-model GPU baseline/observed qualification: 64 snapshots, 640 evaluations and identical CSV/RGB captures. It does not inject or patch target instructions. Actual-game attachment and Color/Output/guide formats, views, pixels and active viewport remain unmeasured. This prepares the next capture; it does not fix the drift or advance the **1 of 8** count. Installed DLLs, INIs and MO2 remain unchanged.

The actual AIO19 debugger capture refused before attachment: a target Windows
attach-entry detour resolves to `S33BUR5CH.asi`; its complete behavior remains
unqualified. A separate reviewed passive reader captured **15 retained host
request snapshots**, all Style 0 / Tone 1 with mask groups off and a null sampled
model callback slot. The [game receipt and next gate](../research/nr/tone-contract/runtime-observer/SKYRIM_CAPTURE.md)
distinguish requested controls from post-callback dispatch controls. Formats,
pixels, guides, viewport and the drift root cause remain open. No game or mod
settings changed; **1 of 8** remains complete.

A fresh exact-AIO19 Ghidra/Capstone trace narrows the native preparation/resolve
route. Thirty prior retained requests predict method 0, with Color/Output
aliasing. A new passive native texture descriptor reader matches **11 owned
textures** and refuses **5 unqualified/inconsistent cases**. The
[research checkpoint](../research/nr/tone-contract/runtime-observer/NATIVE_METADATA.md)
keeps conditional static routing separate from actual GPU execution. Actual
Skyrim texture metadata is the next game gate; production tone handling and
installed settings remain unchanged. **1 of 8** remains complete.

The owner now confirms **Raz Style 0 / Tone 1 has no drift**. The
[source comparison](../research/nr/tone-contract/RAZKOLBAS_COMPARISON.md)
finds SDR RGBA8 Color/Output with the same RTX40 model versus Dva's linear FP16.
An opt-in [SDR-byte trial](../research/nr/tone-contract/SDR_BYTES_TRIAL.md)
preserves the original encoded color through NR, full tone/styles, alpha and
retained ownership. FSR uses its ordinary color decode; the default FP16 path
is unchanged. The older synthetic RGBA8 fixture still drifted. The first Skyrim camera check now reports apparent stability with active NR;
broader acceptance remains required. **1 of 8** remains complete.

Performance P0 now has optional bounded CPU/wait/Flush/descriptor observations and real retired D3D11/D3D12 timestamps for the seven prepared Before NR phases. The isolated executable admits the exact AIO19 model through a research-only generated catalog; production runtime pins are unchanged. The [clean baseline](../research/nr/performance/baseline.json) contains 24 synthetic 2560×1440 timing runs (7,200 measured sources). Vendor medians span 6.80–7.15 ms across both models; alternating transaction medians are approximately 10.0–10.2 ms. Both clean correctness runs preserved all 1,843,200 alpha pixels, including a live bypass/re-enable. Standard NR-enabled validation passed 165/165; Standard NR-disabled passed 148 initially plus all seven previously skipped fixture cases on a targeted eight-case rerun; standalone passed 29/29. Three Graphics Tools cases remain excluded. This is evidence to continue the queue/retained-slot plan, not confirmation of a Skyrim FPS gain. [Qualification and build limits](../research/nr/performance/validation.json) distinguish clean probe captures from precommit product builds. FSR stage timings, matched gameplay, the camera-motion tone root cause and true After remain open. Normal game telemetry and installed settings are unchanged. The completion count stays **1 of 8**.

| Task | Status | Evidence or next requirement |
| --- | --- | --- |
| 1. Post-SR real color/guide/source contracts | Revised; not qualified | Reuse independent renderer and retained real-source contracts; qualify display extent, guide scaling and explicit NR/FG color transfer. Earlier generated-guide/output research is deferred. |
| 2. Catalog, direct owner and compatibility loading | Complete for standalone scope | Exact 65-ID catalog, locked-file hashes, narrow IAT shim; 14/14 NR + legacy contract tests. Reviewed clean-source RTX 40 30-frame RGB readbacks and retired teardown pass. |
| 3. Shared one-pass stage and Before integration | In progress | Owned packet/history, typed parameters and shared Stage implemented. Native linear FP16 D3D11 bridge passes 240 frames with live off/on, exact alpha and real reader retirement. Explicit SDR color preparation and native Before call sites are connected to DLSS/DLAA and FSR. Skyrim FSR source processing is observed; reduced-model reconstruction and moving-scene quality remain open. |
| 4. FSR post-SR NR | Pending real-source qualification | One source evaluation after SR; retained FG color must be recaptured/replaced from enhanced output before submission. |
| 5. DLSS/DLAA post-SR NR | Pending real-source qualification | Feed enhanced real HUD-less output to normal FG tags; suppress duplicate legacy NR and support FG off. |
| 6. Live controls/lifecycle | Before trial integrated; post-SR switches pending | One authoritative setting; Before/After-upscaling changes require source reader retirement and coherent NR/FG history reset. |
| 7. Clean matrix/packages/final review | Pending implementation | Current checkpoint review is narrower than the future whole-branch review. |
| 8. Separate Skyrim acceptance | Processing smoke observed; visual acceptance open | Corrected Before trial loads a save and resumes NR after live toggles. SDR-byte Style-0 camera check reports apparent stability; broader scene/style/provider acceptance remains open. |

RTX 20/30/50 hardware is NOT RUN. AMD is explicitly unsupported. Current qualified driver core is pinned; other cores are unqualified. A successful catalog selection is not a claim of GPU output. Details and bounded receipts: [runtime checkpoint](../research/nr/runtime-catalog/README.md).

Checkpoint review found three issues and all were fixed: CNG backing storage lifetime; RGBA hashes admitting alpha-only changes; and partial RGB writes admitting untouched channels. Two output regressions were observed RED→GREEN. The buffer-lifetime finding follows the CNG API ownership requirement; no deterministic runtime RED was claimed. The final product-tree NR/legacy contract run passed 14/14, without skips.

The shared Stage checkpoint adds a private submission fence, pending packet/descriptor ownership, alpha restoration and terminal retention on uncertain GPU work. A delayed second-queue output copy prevented ticket and feature retirement until its real completion fence advanced. Its 240-frame fixture preserved 55,296,000 source-alpha pixels. These are synthetic real-source tests with static depth and zero motion; generated-frame guides and moving-scene quality remain unqualified.

A fresh Stage review found stale tickets could collide when an owner address was reused. Sealed tickets now retain a unique identity allocation. The same issue was reproduced and fixed for history decisions. Both deterministic same-address regressions were observed RED→GREEN. The first native Before bridge also preserved 13,824,000 alpha pixels across 240 D3D11 round trips, evaluated exactly 232 frames, bypassed eight with every pixel unchanged, and produced 232 distinct RGB hashes. No Skyrim/MO2 settings or installed DLLs were changed.

Clean-source receipts for stage, native bridge and unshimmed control are saved in the [runtime checkpoint](../research/nr/runtime-catalog/README.md). The latest product-tree NR/legacy contract run passed **21/21**, without skips. This is a partial Task 3 checkpoint; the completed count stays **1 of 8**.

The first opt-in **native SDR Before** game trial is now wired. It decodes an explicit SDR source to linear FP16, runs one direct NR pass, restores source alpha and encodes back before either SR provider sees it. After-FG/reduced-model/HDR combinations are unavailable. The accepted FSR Native AA and FG settings are used for this local trial; model training-domain/visual quality remains awaiting Skyrim acceptance.

The prepared source test passed 240 frames with 13,824,000 exact UNORM alpha pixels. The caller's bound RTV was restored. The retained game-source host separately passed 240 frames including off/on and resize, preserving 8,640,000 alpha pixels; the off source is unchanged. Only RTX 4080 SUPER output is observed. Three-family runtime files are individually pinned; AMD remains unsupported.

A fresh review of the bounded Before trial found four important integration errors: continuous SR resets when NR was unavailable, a custom AMD resize path ignoring retirement failure, missed same-camera cuts before NR, and processing fallback frames without a proven HUD-less handoff. Failure-order/world-admission/camera regressions and the unavailable-host regression were observed RED then GREEN. Failed inspection diagnostics are retained. This review does not qualify true After FG or replace the future whole-NR branch review.

Standard product validation passed **158/158** checks, with three Graphics Tools dependent checks explicitly excluded (NativeUIComposition, NativeUIBlendState, NeuralPeripheralPixels). The standalone runtime suite passed **23/23**, without skips. Standard NR-disabled product compilation also passes. A [clean-source 240-frame host receipt](../research/nr/runtime-catalog/rtx40-1077266-before-host.json) now records commit 1077266, all compiled input hashes, actual RTX 4080 SUPER adapter identity, 239 evaluations, exact alpha, an unchanged bypass and reader/runtime retirement. The local three-profile package is pinned and validated. It is installed as `TRP NR Before - DvaKolbas trial` in V5.4 NO-LORE; previous AIO19 profile state is backed up, previous mods are retained, and MO2 launch settings are unchanged. Completed full-plan milestone count remains **1 of 8**; first Skyrim Before acceptance is the next bounded gate.

Final Before-trial targeted verification passed **30/30** NR, shared-world, camera/policy, startup and UI/settings tests. The clean rebuilt host again passed 240 GPU frames. First Skyrim launch is now required; no gameplay NR acceptance is claimed.

The first two Skyrim Before attempts completed save loading but stopped rendering at the initial NR world source. The installed ReShade DLL reproduced a completed-fence identity rejection: its host wrapper differs from the native device returned by fences. The shared Stage now anchors native fence ownership to its own private host-created reference, with contract-device binding and strict completion/foreign-device rejection. The [branch review response and failure analysis](NR_BRANCH_REVIEW_2026-10-03.md) preserves verified recommendations and remaining qualification gates. A corrected Skyrim loading/visual retest is still required; **1 of 8** remains complete.

The next run successfully evaluated NR, then stopped at FSR's strict command/resource ownership check. A new combined actual-ReShade NR + FSR SR/FG fixture reproduced this separate failure on source 1. NR now retains the presenter's existing D3D12 device rather than lazily creating another injector proxy. Both Performance/SRGB and NativeAA/Gamma22 combined routes pass 176 sources, including 170 NR evaluations, live/menu bypasses, FG callbacks, unchanged UI/foreground, two resize cycles and two suspension cycles. The runnable product suite passes **160/160** in 120.25 seconds, with the same three unavailable Graphics Tools checks explicitly excluded; standalone NR passes **24/24** without skips. Skyrim loading/visual acceptance remains pending and the completed milestone count remains **1 of 8**.

Clean `7eb33a7f3ebb` was rebuilt and passed the same **160/160** checks in 122.35 seconds; Standard NR-disabled compilation also passes. The validated DLL and manifest are installed, with all other trial assets, the INIs and MO2 settings preserved. [Combined RED/GREEN, build and installation receipts](../research/nr/runtime-catalog/fsr-ownership-fix-7eb33a7.json) retain qualified scope. The next gate is the user's save-load and Native AA NR/FG visual check; **1 of 8** remains complete.

The corrected build now loads the save and evaluates thousands of native Before world sources. Three live NR off/on cycles resume successfully, with temporal FSR and successful FG callback/submission records. This is a [loading/processing smoke checkpoint](../research/nr/runtime-catalog/skyrim-before-smoke-7eb33a7.json), not visual acceptance: the user subsequently reports that the image changes with camera rotation while NR stays active. Zero appearance presets were loaded, and settings/reset counts stayed stable during the reported period. The FSR community NR path does not invoke the weather-preset controller.

An [isolated motion probe](../research/nr/motion-probe/README.md) recorded 840 sources, including 720 NR evaluations: stationary/pan/stationary sequences, correct/reversed/zero guides, an unchanged off control, faster pans, and a research-only AIO Backbuffer fallback comparison. Correct-guide enhancement did not collapse in this simple scene; reversed/zero guides increased temporal differences. Saved Backbuffer-variant images matched the unmodified Stage exactly. The game defect is unresolved, and no product behavior or installed package was changed. Next: compare the same Skyrim camera movement with NR on/FG off, then NR off; capture actual source/guides if it persists. **1 of 8** remains complete.
