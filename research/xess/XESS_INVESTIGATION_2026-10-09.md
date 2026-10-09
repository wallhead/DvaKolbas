# XeSS investigation — 2026-10-09

## Conclusion and scope

Implement XeSS Super Resolution (SR) first with Intel's official SDK through the existing D3D11/D3D12 interop owner. Add Intel Frame Generation (FG) and XeLL as a separate presentation project afterward. The user selected this order on 2026-10-09.

This report establishes source contracts and static AIO19 evidence. It does not establish gameplay support or successful GPU execution. No AIO19 DLL was loaded, no XeSS context was executed, and no installed Skyrim/MO2 files were changed during this investigation.

## Source identities

| Reference | Inspected revision |
| --- | --- |
| RaZkolbaS, branch `codex/nr` | `6cc2c94d6f4f` (short revision; parent of investigation changes) |
| TRP upstream main | `b169e29006b94aaab43aeeb22bfbed729a4b9bea` |
| Intel SDK stable release | `v3.0.2`, commit `8fe81bdbbaf00b3c1b733fd0d830c333dc84e6f0` |
| AIO19 Hotfix1 archive | 204,042,400 bytes; SHA256 `49e7f7dabf426937915d1aeed664fc40a7cc7d89f42092a69c205b22c4687439` |

Intel's latest published stable release checked on this date is [SDK 3.0.2](https://github.com/intel/xess/releases/tag/v3.0.2). Its release notes include a non-Intel XeLL leak fix when presentation stops and improved handling of Streamline proxies. Those fixes matter for the later FG lifecycle work.

## Upstream

An inspected checkout at the revision above has no XeSS rendering backend. Its XeSS mention in [`NvidiaBaselinePolicy.h`](https://github.com/theosw/theosrenderpipeline/blob/b169e29006b94aaab43aeeb22bfbed729a4b9bea/src/NvidiaBaselinePolicy.h) rejects the legacy experimental capability-probe option. That is not a working SR or FG implementation to cherry-pick.

RaZkolbaS likewise has no XeSS entry in `UpscaleType`, no XeSS backend adapter, and no XeSS presentation owner. Its existing interop, colour conversion and source-frame abstractions are reusable integration infrastructure, not completed XeSS support.

## AIO19: verified findings

The archive's four runtime members are byte-identical to Intel's corresponding official SDK 3.0.2 Git blobs:

| Runtime | Bytes | Official Git blob SHA1 |
| --- | ---: | --- |
| `libxess.dll` | 77,795,704 | `548837c12b215d189884d54db0a480cd3bf2bfd1` |
| `libxess_dx11.dll` | 156,016 | `1e0247ba23a841ff514841ff963b72b636802a4e` |
| `libxess_fg.dll` | 22,957,432 | `57a5da469d79e8dceea6af01961030219d391cdf` |
| `libxell.dll` | 415,368 | `c321c6c6cd148dc614e6a339a27976ce0874d2e7` |

The official payload is available at the [pinned SDK `bin` directory](https://github.com/intel/xess/tree/8fe81bdbbaf00b3c1b733fd0d830c333dc84e6f0/bin). The verifier streams archive members without retaining another large DLL copy. It checks each member's size and Git blob identity, and records SHA256 as well.

Capstone disassembly bounded by PE unwind entries confirms API-name references passed to the actual `GetProcAddress` import in `PDPerfPlugin.dll`:

- SR loader fragment `0x11f310..0x11f880`: D3D11 create/execute and D3D12 create/init/execute.
- Fragments `0x84f50..0x852e8` and `0xb5050..0xb5464`: optional `xefgSwapChainSetNumInterpolatedFrames` resolution.

AIO19's INI also names XeSS upscaling and XeSS FG modes. Its broader API string inventory contains XeLL and FG resource-tagging names. The seven disassembly witnesses are stronger evidence than strings alone, but still only establish loader behavior. They do not prove the selected runtime path was used in a frame.

Optional FG frame-count symbol resolution does not prove multi-frame generation on NVIDIA/AMD. No private AIO19 implementation is needed to use the unmodified public SDK.

### Follow-up: actual static SR dispatch

Further Capstone tracing on the same pinned `PDPerfPlugin.dll` establishes the function-table chain beyond symbol loading:

1. The XeSS wrapper constructor at `0x11f880` stores API-table address `0x137ab50` into holder `0x1375e40` at `0x11f9b9`.
2. The loader stores the resolved `xessD3D12Execute` pointer into `0x137ac20` at `0x11f854`.
3. The execution method's D3D12 fragment loads the holder at `0x120135`, selects table offset `0xd0` at `0x12013c`, then calls that pointer at `0x120152`. The slot identity agrees exactly: `0x137ab50 + 0xd0 = 0x137ac20`.
4. The method starts at `0x11ff30`, referenced by the XeSS wrapper's virtual-method table at `0x12fbff8`. Its platform selector distinguishes the D3D11 branch from the D3D12 branch.

The D3D12 fragment constructs the public `xess_d3d12_execute_params_t` block at `rbp-0x39`. Mapping stores against the pinned public header gives these static observations. `frame` and `wrapper` below are names assigned for review, not recovered source identifiers:

| SDK field | Observed source/store |
| --- | --- |
| Colour resource | `frame+0x08` |
| Velocity resource | `frame+0x10` |
| Depth resource | `frame+0x18` only when `wrapper+0x35` is zero; otherwise initially null |
| Exposure / responsive-mask textures | Null in this fragment |
| Output resource | `frame+0x30`, falling back to `frame+0x28` when null |
| Jitter X/Y | Float values forwarded from `frame+0x44/+0x48`, without a sign change here |
| Exposure scale | Constant `1.0f` |
| History reset | Boolean-normalized byte from `frame+0x54` |
| Input width/height | Float-to-integer conversion from `frame+0x38/+0x3c` |
| Input colour / motion / depth bases | Forwarded from `frame+0x70/+0x74`, `+0x80/+0x84`, `+0x78/+0x7c` respectively |
| Command list | `frame+0x68`, passed as the SDK's second argument |

The initializer at `0x11fd30` also conditionally constructs the SDK high-resolution-motion, auto-exposure, LDR, inverted-depth and jittered-motion bits from its generic configuration. This confirms those are explicit wrapper choices; it does not establish their values during a user's game session.

The verifier now records the full D3D12 fragment's instruction bytes and verifies the table/slot/call chain. Static call-site identification is not proof that a particular gameplay frame executed it. Still unverified: the upstream construction of `frame`, actual resource formats and colour encoding, original jitter signs and motion scales, selected configuration flags, barriers and downstream reader retirement. Those are the next RE targets alongside the official SDK implementation work.

### Follow-up: game-side input preparation

The pinned `SkyrimUpscaler.dll` has delay imports from `PDPerfPlugin.dll`. The verifier now resolves their actual IAT slots and checks four call sites, rather than relying on API-name strings:

| API | Game-side call RVA | Delay-import slot |
| --- | --- | --- |
| `GetJitterOffset` | `0x1b48cc` | `0x538370` |
| `EvaluateUpscaler` | `0x2e983c` | `0x538320` |
| `SetMotionScaleX` | `0x2f95e2` | `0x538308` |
| `SetMotionScaleY` | `0x2f95ef` | `0x538310` |

In the jitter hook, `GetJitterOffset` supplies `hx/hy`. Instructions `0x1b48eb..0x1b493a` use verified constants `-2`, `+2`, and sign-bit XOR masks. They store `-2*hx/W` and `+2*hy/H` at hook-object offsets `+0x44/+0x48`, and `-hx/-hy` at scene-owner offsets `+0x08/+0x0c`. The resolution-normalized pair is consistent with camera projection offsets; its complete downstream matrix consumer has not yet been traced. The generic evaluation frame reads the negated pair from owner `+0x08/+0x0c` into frame `+0x44/+0x48`, matching the fields forwarded to the SDK by its wrapper. This is a static convention witness, not an assertion about every camera path or selected runtime branch.

At `0x2f95bb..0x2f95ef`, owner fields `+0x278/+0x27c` are converted to floats, stored as the motion scales and passed to the public motion-scale setters. The same dimension fields populate input width/height in the generic SR evaluation frame at `0x2e96ac..0x2e96ff`. This ties the scales to input dimensions rather than an assumed display size on this path. It does not prove the units/signs of the underlying motion texture or whether it has been dilated.

The game-side evaluation method prepares colour/depth/motion copies through its D3D11 context and includes optional processing branches before constructing the generic frame. The shared D3D12 backend's SR dispatch at `PDPerfPlugin` RVA `0x114d95` copies the generic frame into the selected upscaler's virtual execute method. These inspected dispatch/copy blocks contain no explicit gamma-decoding arithmetic, but that does **not** establish an encoded-colour SDK input: resource creation and optional shader paths are not fully resolved. The next colour investigation must follow those paths and texture descriptions before assigning an encoding.

The game-side resource-preparation fragment at `0x2f8f2e..0x2f8f78` copies a runtime-owned descriptor region into the texture descriptor later used to validate the colour resource; its format field is inherited rather than set to a single literal in that block. This prevents assigning an AIO19 colour format from a nearby immediate constant alone. Identifying the selected original render target and tracing the optional shader branches remain necessary to establish its actual encoding.

The SR design's requirement for an explicit jitter/motion adapter is supported by this RE. Its linear-colour conversion remains grounded in Intel's contract, not a guessed AIO19 format. New instruction bytes, constants and import identities are saved under `gameInputs` in the witness report. Only static analysis was performed.

### Reproduce

Requires Python with `pefile` and `capstone`, 7-Zip, the pinned archive and its already extracted caller DLLs. Paths below assume the original local research layout; the script accepts replacement paths.

```powershell
C:/Python314/python.exe tools/xess/Inspect-Aio19Xess.py `
  --archive 'C:/Users/user/Downloads/SkyrimUpscalerAIOBuild19-Hotfix1.7z' `
  --extracted out/research/aio19/extracted `
  --output research/xess/aio19-sdk-witnesses-2026-10-09.json
```

Observed result: `PASS: 4 official SDK runtime matches; 7 verified loader witnesses; D3D12 SR dispatch chain; game-side input witnesses`. Detailed hashes, instruction bytes and RVAs are in [the witness report](aio19-sdk-witnesses-2026-10-09.json). A different input artifact is rejected rather than interpreted using these RVAs.

## Official contracts affecting our integration

Intel's [SR guide](https://github.com/intel/xess/blob/8fe81bdbbaf00b3c1b733fd0d830c333dc84e6f0/doc/xess_sr_developer_guide_english.md) restricts D3D11 SR to Intel Arc or later. D3D12 enables the cross-vendor route; the documented non-Intel requirements include SM 6.4 and accelerated DP4a. Therefore the proposed common backend uses D3D12 on the same adapter as Skyrim's D3D11 device, including on Intel.

SR expects linear colour and reconstructs linear output. Low-resolution motion is current-to-previous, measured in input pixels, without dilation; depth is required for that path. Camera jitter needs an explicit convention adapter. SDK alpha output is 1, so HUD alpha must remain separately owned. These constraints make the existing ENB SDR conversion and UI separation relevant, but passing encoded ENB colour unchanged is incorrect.

The [public SR header](https://github.com/intel/xess/blob/8fe81bdbbaf00b3c1b733fd0d830c333dc84e6f0/inc/xess/xess.h) provides AA and several quality modes, size queries, explicit depth/motion flags and logging. Native maps to AA. Query optimal input sizes rather than copying FSR ratios. The [D3D12 interface](https://github.com/intel/xess/blob/8fe81bdbbaf00b3c1b733fd0d830c333dc84e6f0/inc/xess/xess_d3d12.h) requires correct input/output states and application-owned synchronization.

Intel's [FG guide](https://github.com/intel/xess/blob/8fe81bdbbaf00b3c1b733fd0d830c333dc84e6f0/doc/xess_fg_developer_guide_english.md) requires D3D12 presentation plus a working XeLL context on all vendors. Non-Intel FG permits one generated frame. The proxy requires exclusive swap-chain ownership; it cannot simply be layered over our existing FG owner. UI composition is opt-in: default UI interpolation would violate our intended pipeline. XeSS FG therefore follows SR as a separate project with its own latency, lifetime and UI tests.

## Integration decision

See the [SR design](../../docs/superpowers/specs/2026-10-09-xess-sr-design.md). First prove the official SR runtime on the local RTX 4080 SUPER with synthetic temporal scenes. Then integrate Skyrim with FG/NR off, followed by NR and existing FG providers. Intel, AMD and other NVIDIA families require real GPU qualification; simulated device descriptions cannot prove SDK execution or image quality.

The initial SR package needs `libxess.dll` (about 74.2 MiB uncompressed). A unified D3D12 path does not need the Intel-only D3D11 dispatcher. FG and XeLL payloads are deferred to their own package stage. No additional large runtime copies were retained by this investigation.
