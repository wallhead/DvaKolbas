# TRP source-owned AMD neural rendering engine

Date: 2026-10-05. Branch: `codex/amd-nr-engine`.
Status: approved by the user on 2026-10-05; standalone foundations implemented and reviewed; canonical C512 projection storage codec implemented; C512 residual/output boundaries recovered statically. Full inference and TRP integration remain pending.

## Intent and scope

Build our own inference implementation from the recovered numerical behavior, then connect it to Theo's Render Pipeline. The supplied `winmm.dll` is a local analysis/comparison reference. The product will execute our C++ and compute shaders. It will read weights selected by the user, without including the supplied weights, DLL, disassembly, decompiled source, or GPU objects in the package.

The user selected all three proposed AMD generations: RDNA2, RDNA3, and RDNA4. These are implementation and validation targets, not established compatibility. The first game route is one NR pass after the active upscaler, with native input resolution and HUD composition. Existing NVIDIA rendering and frame generation continue through their existing route. AMD frame generation, pre-upscale NR, HDR, and multiple NR passes are separate extensions after this route works.

Success requires an independently executed whole network, controlled numerical comparisons, an AMD presentation path that survives device/resource lifecycle events, and actual tests on each target generation. Loading weights or returning a copied image is not completion.

## Approach selection

**Recommended: D3D12 compute baseline with explicit numerical formats.** Own HLSL kernels and a C++ graph runner use the same D3D12 device/queue as the transport. FP8 is encoded storage with explicit software decode/encode; FP16 storage and required rounding boundaries are explicit. This provides one correctness path for all three target generations and keeps inference retirement on a single GPU queue.

**Alternative: own HIP kernels.** This matches the reference's execution API and facilitates architecture-specific optimizations, but introduces a second runtime, cross-API resource ownership, and a narrower official Windows SDK support matrix. Some RDNA2 products have runtime support without full HIP SDK support. HIP is a possible later optimized backend after the portable numerical path is validated.

**Alternative: DirectML matrix operators plus custom shaders.** This can accelerate the matrix portions while custom kernels implement layout, activation, attention, and packing. Its operator-selected accumulation behavior must be compared at intermediate checkpoints. It is not selected as the initial numerical reference because operator fusion can obscure rounding differences.

The supplied runtime wrapper approach is superseded by the user's request for our own engine.

## Recovered facts and uncertainty

The exact local weights archive contains 153 records covering blocks 0–70. Its payload starts at byte 5673; all records are unique, contiguous, and close exactly at EOF. Record boundaries are not enough to describe each packed internal tensor.

The host orchestrator exposes pre processing, encoder groups with 32/64/128/256/512 channels, eight 1024-channel ViT blocks, and a mirrored decoder ending in post processing. Static registration maps all 182 host kernel tokens to embedded AMDGPU symbols. New pre/post variants consume a 192-byte `VarParams` structure; the older named pre/post variants use 80-byte structures. Those ABIs must not be conflated.

The CPU lookup initializer decodes E4M3 with exponent bias 7, subnormal unit 1/512, finite maximum 448, signed zero, and positive quiet FP16 NaN for both `0x7f` and `0xff`. The reconstructed FP32-to-FP16 conversion rounds to nearest, ties to even and explicitly quiets NaNs. The local numerical probe covers all 256 decode codes and 51,230 conversion inputs.

Default extent policy rounds both axes up to 64, with a minimum of 320. If both rounded axes are divisible by 256, the reference adds 64 to **height**. For 1920×1080 it chooses 1920×1088, with a deepest grid of 32×20. Alternative modes use steps of 8 or 128. Public ports use different geometry policies; their images cannot be assumed identical.

Registered noise generators accept an FP16 destination, two dimensions, and a seed. Their integer hash, transcendental approximations, output arrangement, and callers require explicit recovery before their contribution can be reproduced. The host contains history and exposure controls. Their exact formulas and guide-resource use must be established through data flow, not guessed from packet field names.

At initial design review, incomplete contracts included internal tensor shapes/offsets, FP8 encoding and saturation, accumulation order, window/index permutations, pooling and skip mappings, input/output transforms, exposure, history, and noise. Each implementation stage must close its contracts using static evidence and controlled reference checkpoints. It must not invent operations to fill gaps.

Subsequent C512 recovery closes the projection record indexing and the numerical
policy around its residual initialization and output storage. The owned storage
codec is implemented; residual math and the encoder are not yet product
operations. The [residual/output contract](../../amd-nr-c512-residual-output.md)
specifies nearest-even E4M3 encoding, signed zero, saturation to finite ±448,
and canonical positive NaN `0x7f`. Software conversion must cover every FP32 bit
pattern without out-of-width shifts. The portable baseline uses our own integer
encoder without a HIP/native FP8 dependency. This closes the C512 output policy,
not all network output transforms or general matrix reduction equivalence.

## Components and boundaries

1. **Model archive reader.** Validate header, directory length, counts, names, overflow, range overlap, and EOF before allocation or upload. Bind the exact known model identity to a versioned logical schema. Unknown identities, unexpected record lengths, and incomplete schemas return an explicit unsupported-model result. Do not reproduce the reference's private GPU packing as the public archive format.

2. **Numerical reference.** An independent CPU runner implements documented tensor operations and logical layouts. Each packed record receives a checked view with proven offsets, dimensions, strides, and encoding. Distinguish FP8 decode from encode, FP16 rounding from ordinary FP32 arithmetic, and exact integer indexing from floating approximation. Generated test tensors include impulses, constants, boundary coordinates, and fixed-seed random inputs.

3. **Compute kernels.** Start with portable byte-addressed FP8/FP16 storage and FP32 arithmetic with explicit rounding where required. Avoid `min16float` as a promise of fixed precision. Use the existing shader-compilation toolchain for the initial baseline where it can express the contract; any later DXC/native-16-bit path is independently gated by device capabilities and numerical comparisons. Optimize one already-validated operation family at a time.

4. **Graph and memory planner.** Own the logical graph, tensor lifetimes, skip edges, history, weights, PSOs, and descriptors. A block is enabled only after its schema and CPU/GPU comparison pass. Allocate a bounded resident arena; reuse scratch after its last consumer, preserve skip tensors until consumption, and retain every submitted resource until its fence completes. Tensor metadata is independent of the reference's global addresses and pointer offsets.

5. **Standalone evaluator.** Accept explicitly described frame resources or local fixtures, run the source-owned graph on a chosen adapter, and export bounded intermediate comparisons and final image diagnostics. It is a separate executable from Skyrim and is not a pass-through proxy. It uses our own entry points and source-compiled shaders.

6. **TRP frame transport and neural session.** Add a vendor-independent host selection before swapchain initialization. Reuse proven same-adapter D3D11/D3D12 resource sharing and lifecycle logic where possible, without making Streamline Reflex/PCL/DLSS-G availability an AMD prerequisite. The session boundary exposes prepare, record, submitted-fence retirement, history reset, and status. It does not copy the DLL's Record/Notify/private-global ABI.

## Frame and lifecycle contract

Choose the presentation owner once before creating its swapchain. NVIDIA startup follows the current host. The AMD route owns its D3D12 resources and uses the existing TAA or selected Community Shaders upscaler; it does not apply NVIDIA DLSS jitter/TAA ownership.

At the NR boundary, transport HUD-less scene colour plus any proven required guides to the same adapter. Frame metadata describes active extent, processing extent, colour transfer/range, depth convention, motion units, jitter, exposure, and reset state explicitly. The model input codec uses only the inputs that the recovered contract actually consumes. It must not silently reinterpret a resource format.

Record conversion, the validated full graph, output conversion, and composition in a host-owned command list with explicit barriers. Submit once. A returned job owns the command allocator, descriptors, scratch, input/output, and history until its D3D12 fence retires. Compose native UI after NR. An output becomes visible only after the corresponding job completes.

Resize, loading transitions, scene discontinuities, model changes, and device recreation invalidate history and require retirement of old resources. Repeated off/on must not leave orphaned work or reset unrelated settings. On validation/allocation/dispatch failure, present the current unmodified upscaler output and expose the reason; never feed stale output into a temporal accumulator. If the queue cannot be drained, retain resources rather than free GPU-visible memory.

## Hardware coverage and failure behavior

The baseline performs runtime capability checks, format-support checks, same-adapter identity checks, resource-size overflow checks, and budget checks. Architecture-specific fast paths are optional and cannot remove the baseline from RDNA2/3/4. Wave size, native FP16, matrix instructions, and vendor features are checked capabilities rather than assumptions inferred from a marketing name.

Coverage means tests on representative RDNA2, RDNA3, and RDNA4 devices plus cross-generation numerical comparisons. Other AMD hardware may pass capability checks, but does not receive an untested support claim. The local workstation's RTX 4080 SUPER and AMD integrated adapter can help test portability and lifecycle behavior; they cannot establish acceptance on the requested Radeon generations.

There is no frame-rate promise at this stage. Correctness, peak memory, per-stage time, and end-to-end time are measured separately. A slow validated baseline is a starting point for optimization, not release-ready performance.

## Validation and completion gates

**Model/numerical gate:** malformed archive fixtures fail before upload; the known archive accounts for every record; every internal view closes within its record. Format probes cover signed zeros, subnormals, saturation, infinities/NaNs, and halfway rounding. Indexing probes cover non-square extents, padded windows, crops, and all skip edges.

**Operation gate:** every kernel family has a CPU reference and controlled original-runtime checkpoints or a documented static contract. Require exact integer/layout results and exact storage bits where rounding is fully specified. Floating approximations receive explicit measured tolerances per operation, with maximum error, non-finite counts, and error accumulation reported. Whole-frame visual similarity alone cannot pass this gate.

**Graph gate:** checkpoints include blocks 0, 4, 8, 14, 22, 30, 31/38, 39/47, 55, 61, 65, 69, and 70. Use multiple synthetic fixtures and held-out game frames, not one calibrated image. Verify fixed-seed repeatability, history-reset behavior, and padding/crop effects. No selected block may fall back to a copied reference activation.

**Integration gate:** validate NR off/on, failure fallback, ordinary gameplay, resize, loading, reset, shutdown, UI preservation, resource retirement, and device mismatch. Run existing relevant NVIDIA/lifecycle tests to verify that host extraction did not regress the current route.

**Hardware gate:** standalone and game runs on all three target generations record correctness, driver/device identity, peak allocation, timing, and acceptance limits. Report missing hardware as unvalidated; do not promote a successful compile to working AMD NR.

## Source and evidence handling

The product consists of source-owned C++, HLSL, logical schemas, and tests. Existing TRP source and SDK APIs are reused where appropriate. Any intentionally adapted open-source implementation carries its attribution; downloaded public implementations are research references until such a choice is explicit.

The local RE project, all supplied or extracted binaries, trained parameter data, captured reference activations, disassembly, and decompilation remain under ignored `local/amd-nr-re/`. The design and prose findings may be committed to this separate branch. No package, push, or game installation is part of design review.

## Primary technical references

- [AMD Windows HIP SDK support matrix](https://rocm.docs.amd.com/projects/radeon-ryzen/en/docs-7.1.1/docs/shared/hipsdk/reference/system-requirements.html): distinction between runtime and SDK support.
- [Microsoft DXC 16-bit scalar types](https://github.com/microsoft/DirectXShaderCompiler/wiki/16-Bit-Scalar-Types): native 16-bit compilation and minimum-precision distinctions.
- [MIT source port](https://github.com/lmxxf/dlss5-on-amd-9070xt-porting/tree/fd6e796ecb73c5a8dbeabad92942042950a4339a): external cross-check, not numerical proof for the supplied DLL.

Exact local evidence and reproduction commands are summarized in `docs/amd-nr-research.md`.
