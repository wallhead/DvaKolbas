# Mixed DLSS / FSR FG packed-depth repair

The user reported whole-image shaking after the Round 11 delivery. It persisted
with FG disabled. The game log recorded successful NGX feature creation but no
NGX evaluation, and reported spatial source recovery for unavailable guides.

## Root cause

`UpscalerHooks.cpp` captures Skyrim's packed depth-stencil allocation in the
R24G8 family. The mixed-route admission gate mistakenly required an already
converted R32_FLOAT allocation. It rejected usable source depth before DLSS or
NR evaluation. Spatial scaling retained the real image but did not reconstruct
the jittered camera image. Disabling interpolation left the same source gate in
place, explaining why FG off did not remove the reported shaking.

Earlier actual-runtime fixtures supplied R32_FLOAT depth and missed this source
boundary. Their passing results did not qualify Skyrim's packed-depth path.

## Repair

The source gate shares the depth converter's supported SRV-format mapping:
R24G8_TYPELESS, R32G8X24_TYPELESS, R32_TYPELESS and R32_FLOAT. Packed depth reaches
NGX unchanged; the existing NR/FSR adapters still convert it into shared R32
guides. Exact render extents, single mip/array/sample, SRV binding and producer
device ownership remain checked. Motion must remain R16G16_FLOAT.

Guide recovery logs missing resources or actual extent, format, binding, sample
and device information on the first and every 120th recovery frame, even with
frame-detail logging disabled. The existing recovery counter now includes guide
failures. A resume message requires actual successful temporal DLSS evaluation;
loading-screen spatial scaling cannot report a temporal resume.

## Verification

The WARP regression first failed at
`ReadableProducerDepthReachesTemporalDlssWithFgOnOrOff`. After repair it passed
all four supported source formats at native and scaled output extents, with FG
requested both on and off and nonzero source jitter. Six focused CTest cases
passed: source evaluation, frame copies, external runtime, guide transport,
external presentation ownership and mixed host recovery.

The actual NGX/NR/FSR fixture now uses R24G8_TYPELESS with SRV and depth-stencil
bindings. Packed depth/stencil bytes are uploaded separately and copied as a
whole subresource; the fixture does not call UpdateSubresource directly on a
depth-stencil allocation. Receipts identify this input format, and the runner
rejects fixtures without that evidence.

On the local RTX 4080 SUPER, Native, Quality and Performance each passed 160 NGX
dispatches, 152 NR evaluations, 138 generated callbacks, 134 generated pixel
readbacks, 290 HUD checks, four policy reentries, two pending-reader retirements
and 14 reset reentries. Zero FSR SR contexts or dispatches were observed. The NR
omission control correctly failed with `no actual NR enhancement observed`.
Windows graphics debug layers were unavailable. An independent static review
reported no blocking findings.

These initial runs used the uncommitted repair on baseline 4a48e15, identified by
exact executable hashes in `out/research/packed-depth-2026-10-08-precommit`.
Clean-build receipt links and delivery hashes are recorded after commitment.
Skyrim visual acceptance is still pending; the independent FG plan remains
**5 of 6 complete**.

## Clean delivery

The Universal plugin and actual-runtime fixture were rebuilt from clean source
**d15ffdc4c1e5**. The packed-depth runner passed again at
[Native](packed-depth-recovery/native.json),
[Quality](packed-depth-recovery/quality.json), and
[Performance](packed-depth-recovery/performance.json); the
[NR omission control](packed-depth-recovery/negative-control.json) was rejected
for the intended reason.

Installed in the existing V5.4 NO-LORE mod with backup at
`out/install-backups/independent-fsr-fg-d15ffdc4c1e5`. Independent verification
confirmed the clean embedded revision, exact installed DLL hash, all 113 INI
values and unchanged MO2 profile/launch hashes. Current Backend=FSR, DLSS Quality
and the user's NR/FG preferences were preserved.

DLL SHA256: `4c27a5f592e9af127a5652d150fe54f2852911601c888dba2a5f63228b6da51f`.
The separate 19-entry archive is
`RaZKolbaS DLSS FSR FG NR v1.2 - packed-depth test.zip` in Downloads, with SHA256
`0abee676813e40cb04f0ffeabd059a7a624fd302f0b3396c9ff8495bcf54c3bc`.
Payload inventory, CRCs and portable native defaults passed verification. The
original v1.2 archive and earlier test packages remain intact.

**6 of 6 prepared**, with Skyrim acceptance still pending. Start the same save,
rotate the camera with FG off and on, then inspect the game log for successful
NGX evaluation and generation rather than spatial recovery before recording
gameplay completion.
