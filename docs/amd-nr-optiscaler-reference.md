# OptiScaler AMD NR reference audit

Date: 2026-10-05. User-provided reference:
[3zwr1/AMD-NR---OptiScaler](https://github.com/3zwr1/AMD-NR---OptiScaler).
The audit pins commit `f0c0232a2384f1fc9ee0167dcabf2dbe34fd7708`. Ten public
text files and the complete 800-entry tree are cached under ignored
`local/amd-nr-re/optiscaler-public/`, with sizes and hashes in `manifest.json`.
No external DLL, neural assets or compiled kernels were fetched or executed.
No source from this repository was imported into the product.

## Published source and missing runtime

The [project notice](https://github.com/3zwr1/AMD-NR---OptiScaler/blob/f0c0232a2384f1fc9ee0167dcabf2dbe34fd7708/AMDNR_NOTICE.txt)
describes the repository as OptiScaler host source, with separately packaged
neural runtimes. Its lmxxf runtime directory publishes a license, the host API
header and a device-identity header. The complete tree contains no `.hip`, `.cu`,
`.hsaco`, `.pak` or `lmxxf_gfx11_compat.h` file. This checkout does not supply the
full inference implementation or the advertised RDNA3 compatibility source.

The [lmxxf host implementation](https://github.com/3zwr1/AMD-NR---OptiScaler/blob/f0c0232a2384f1fc9ee0167dcabf2dbe34fd7708/main/AMDNR-source/optiscaler/dlssnr/lmxxf/LmxxfBackend.cpp)
loads a runtime DLL and retrieves `LmxxfNrGetApi`, including asynchronous enqueue,
abandonment and readiness entries. Those calls delegate inference to the separate
package. Rebuilding this host alone would not create our own network executor.

## Hardware and matrix relevance

The [GPU classification source](https://github.com/3zwr1/AMD-NR---OptiScaler/blob/f0c0232a2384f1fc9ee0167dcabf2dbe34fd7708/main/AMDNR-source/optiscaler/dlssnr/amd/GpuSupportRules.h)
offers both runtimes for gfx12 and desktop gfx1100/1101/1102, marks selected
12-or-more-CU APUs experimental, and rejects gfx10/gfx103 RDNA1/2. These are
its host policy and runtime claims, not acceptance established by our audit.
They do not satisfy our RDNA2/3/4 target coverage.

The [RDNA3 backend notice](https://github.com/3zwr1/AMD-NR---OptiScaler/blob/f0c0232a2384f1fc9ee0167dcabf2dbe34fd7708/AMDNR_RDNA3_BACKEND.txt)
describes FP8 matrix work carried through FP16 matrix units using a compatibility
header and architecture-specific HIP modules. This supports investigating a
later gfx11 optimization after our own numerical baseline is established. It
does not demonstrate equivalence for our supplied C512 WMMA reduction or provide
a portable RDNA2 implementation.

Our selected D3D12 baseline therefore remains relevant: decode storage in
software, implement matrix arithmetic with an explicit reduction/rounding
contract, then optimize verified operations for supported hardware. Our portable
E4M3 output encoder is now implemented and independently tested; matrix execution
remains a separate gate.

## Graph and transport lessons

The [public API header](https://github.com/3zwr1/AMD-NR---OptiScaler/blob/f0c0232a2384f1fc9ee0167dcabf2dbe34fd7708/main/AMDNR-source/optiscaler/dlssnr/lmxxf/runtime/LmxxfNrApi.h)
separates preparation, producer recording, HIP enqueue, consumer recording,
readiness and retirement. Its full-network flag runs all 71 blocks, while its
default schedule skips 42, 43 and 46. A schedule change rebuilds the network and
invalidates prior job/history state. Useful comparisons must therefore record
geometry, graph selection, history state and completion before interpreting
image differences. This API is not our supplied binary's graph reconstruction.

The [bridge source](https://github.com/3zwr1/AMD-NR---OptiScaler/blob/f0c0232a2384f1fc9ee0167dcabf2dbe34fd7708/main/AMDNR-source/optiscaler/dlssnr/amd/AmdBridge.cpp)
handles a runtime-specific proxy-device/queue mismatch explicitly before backend
creation. Our same-queue D3D12 engine should likewise validate device identity
and preserve submitted resources until completion. We do not adopt its private
offsets or introduce another inference runtime.

The [private-layout table](https://github.com/3zwr1/AMD-NR---OptiScaler/blob/f0c0232a2384f1fc9ee0167dcabf2dbe34fd7708/main/AMDNR-source/optiscaler/dlssnr/amd/AmdLayout.h)
contains ten accepted runtime SHA-256 identities. None equals our supplied DLL
identity `195c4a891b6eac4c1cb7671e10ff62bbbe2b17f1dfae1344dc5a6714e4775721`.
Its RVAs and fields cannot be transferred to that binary without new evidence.

The host notice labels its own code GPL-3.0-or-later; the separate RDNA3 backend
notice labels that backend MIT. This audit records the stated boundaries; there
has been no code adaptation or dependency addition.

## Next work toward our own running NR

1. Close C512 fragment/Tin/spatial addressing and matrix reduction equivalence,
   then implement an independently compared projection using our owned weight
   view and portable encoder.
2. Recover remaining operation families and the full 71-block graph, including
   input/output transforms, exposure, noise and temporal inputs.
3. Run a complete standalone frame through our own kernels and collect bounded
   intermediate comparisons before enabling the TRP frame route.
4. Integrate scene transport, HUD composition, reset and fence retirement;
   validate on representative RDNA2, RDNA3 and RDNA4 devices.

The reference informs these steps but does not eliminate them. Model status
remains `KnownArchiveIncompleteSchema`, `inference=unavailable`.
