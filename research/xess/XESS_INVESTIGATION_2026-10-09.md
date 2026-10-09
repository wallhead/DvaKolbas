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

Not yet established by RE: AIO19's actual jitter signs, motion scales, initialization flags, colour conversion, resource formats, or reader retirement rules. Optional FG frame-count symbol resolution does not prove multi-frame generation on NVIDIA/AMD. No private AIO19 implementation is needed to use the unmodified public SDK.

### Reproduce

Requires Python with `pefile` and `capstone`, 7-Zip, the pinned archive and its already extracted caller DLLs. Paths below assume the original local research layout; the script accepts replacement paths.

```powershell
C:/Python314/python.exe tools/xess/Inspect-Aio19Xess.py `
  --archive 'C:/Users/user/Downloads/SkyrimUpscalerAIOBuild19-Hotfix1.7z' `
  --extracted out/research/aio19/extracted `
  --output research/xess/aio19-sdk-witnesses-2026-10-09.json
```

Observed result: `PASS: 4 official SDK runtime matches; 7 verified loader witnesses`. Detailed hashes, instruction bytes and RVAs are in [the witness report](aio19-sdk-witnesses-2026-10-09.json). A different input artifact is rejected rather than interpreted using these RVAs.

## Official contracts affecting our integration

Intel's [SR guide](https://github.com/intel/xess/blob/8fe81bdbbaf00b3c1b733fd0d830c333dc84e6f0/doc/xess_sr_developer_guide_english.md) restricts D3D11 SR to Intel Arc or later. D3D12 enables the cross-vendor route; the documented non-Intel requirements include SM 6.4 and accelerated DP4a. Therefore the proposed common backend uses D3D12 on the same adapter as Skyrim's D3D11 device, including on Intel.

SR expects linear colour and reconstructs linear output. Low-resolution motion is current-to-previous, measured in input pixels, without dilation; depth is required for that path. Camera jitter needs an explicit convention adapter. SDK alpha output is 1, so HUD alpha must remain separately owned. These constraints make the existing ENB SDR conversion and UI separation relevant, but passing encoded ENB colour unchanged is incorrect.

The [public SR header](https://github.com/intel/xess/blob/8fe81bdbbaf00b3c1b733fd0d830c333dc84e6f0/inc/xess/xess.h) provides AA and several quality modes, size queries, explicit depth/motion flags and logging. Native maps to AA. Query optimal input sizes rather than copying FSR ratios. The [D3D12 interface](https://github.com/intel/xess/blob/8fe81bdbbaf00b3c1b733fd0d830c333dc84e6f0/inc/xess/xess_d3d12.h) requires correct input/output states and application-owned synchronization.

Intel's [FG guide](https://github.com/intel/xess/blob/8fe81bdbbaf00b3c1b733fd0d830c333dc84e6f0/doc/xess_fg_developer_guide_english.md) requires D3D12 presentation plus a working XeLL context on all vendors. Non-Intel FG permits one generated frame. The proxy requires exclusive swap-chain ownership; it cannot simply be layered over our existing FG owner. UI composition is opt-in: default UI interpolation would violate our intended pipeline. XeSS FG therefore follows SR as a separate project with its own latency, lifetime and UI tests.

## Integration decision

See the [SR design](../../docs/superpowers/specs/2026-10-09-xess-sr-design.md). First prove the official SR runtime on the local RTX 4080 SUPER with synthetic temporal scenes. Then integrate Skyrim with FG/NR off, followed by NR and existing FG providers. Intel, AMD and other NVIDIA families require real GPU qualification; simulated device descriptions cannot prove SDK execution or image quality.

The initial SR package needs `libxess.dll` (about 74.2 MiB uncompressed). A unified D3D12 path does not need the Intel-only D3D11 dispatcher. FG and XeLL payloads are deferred to their own package stage. No additional large runtime copies were retained by this investigation.
