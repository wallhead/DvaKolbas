# Revision 3 — 1 October 2026

Added 49 annotated excerpts, 183 selected instructions, 11 constant/string records, a PD API vtable, corrected pipeline metadata, and 16 reference-model tests. Cumulative totals: 128 excerpts, 325 instruction records, 316 distinct addresses.

## Corrections and qualifications

The old section 10.1 field label “+0x20 output” is wrong for DX12 FSR SR. It is reactive; default/preferred outputs are +0x28/+0x30. The DX11 internal bundle is a different layout and is explicitly distinguished.

The host FG reset byte is zero in the normal producer, but the native backend ORs in its own resetPending latch. No “FG never resets” conclusion is valid.

The source clear at host +0x266DB0 resolves SubmitFgIdleWideBackground; it is not a universal temporal-history clear. The scalar math thunk used in bias generation is not conclusively linked to a rebuilt log2f import, so only its observed bounds are reported.

## New analysis

UI-phase viewport/depth-SRV redirection; GetResource reference leak under documented D3D11 semantics; conditional six-stage sampler cache; jitter signs and Halton phase arithmetic; prepared-depth/motion dimensions; depth convention updates and context recreation; success-dependent FG reset acknowledgment; external exposure/T&C bindings; far-plane sanitization.

Fresh GNU decoding validates every new excerpt line. LLVM independently agrees with selected landmark boundaries/bytes. No target DLL or GPU workload executed.

---

# Revision 2 — 1 October 2026

Same original archive and module hashes; new read-only static analysis.

## Added

Mapped the pre-UI hook installer to relocation IDs 79947/82084, displacements 0x16F/0x17A, and host handler +0x19F420. Connected the handler to actual SR +0x19FA14, conditional before-/after-ReShade effects, the effects-execution guard, separate UI allocation, and persistent OMSetRenderTargets redirection.

Separated normal early FSR preparation from the DLSS/FrameWarp late path. Identified the actual registered ReShade event-75 callback +0x284D30, pending-source latch, and the missing-callback clearing path. Preserved the alternate callback clone without treating it as the registered target.

Recovered AMD internal class layouts, their inheritance and embedding offsets. Mapped UI capture inside Present, barriers and copy submission, the separate output/backbuffer indices, six fence roles, buffer reuse, and drain/destroy/resize forwarding.

Traced shared D3D12 fences into D3D11Device5 and shared texture import paths. Added a negative attribution control: the directly observed caller of PD's generic FGFence publisher is in its DLSS-G state path, not proven FSR ownership.

Added 40 disassembly excerpts, 92 instruction records, nine class/inheritance layouts, 22 concrete AMD vtable checks, full input code-range hashes, a machine-readable pipeline map, an expanded CodeView parser, a revised verification tool and runtime breakpoint map.

## Clarified

The final-overlay gate is conditional, not a universal requirement before normal FSR preparation. UI-copy registration is distinct from capture inside Present. “Internal UI double-buffering” is not proof of two private UI buffers: the recovered class has one UI replacement resource and two separate interpolation outputs.

The 16-entry replacement array is capacity, not 16 active backbuffers or 16 generated frames. Source frame ID, presentCount and framesSentForPresentation are distinct. A 1 ms event wait is a retry interval, not an overall timeout. A true return from the drain method alone does not prove actual successful retirement.

## Retained limitations

No DLLs were executed. No game/GPU capture, performance measurement, confirmed live provider selection, full Scaleform flush, per-widget attribution, or universal ENB/Community Shaders ordering was established. The renderer-level hook and AMD lifecycle findings are narrower than those unresolved runtime contracts.
