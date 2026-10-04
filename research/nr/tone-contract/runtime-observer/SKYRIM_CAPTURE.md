# Skyrim AIO19 retained-request checkpoint

2026-10-04. The user started AIO19 in V5.4 NO-LORE. The normal debugger observer failed to open Skyrim (Win32 5); the elevated retry refused **before attachment** because the target `ntdll!DbgUiRemoteBreakin` prefix differed. Read-only tracing found an E9/FF25 relay to the loaded `Stock Game/S33BUR5CH.asi+16DC80`. Its exact disk identity is retained in [the receipt](skyrim-passive-request.json). The hook's complete behavior is unqualified. It was not removed or bypassed, and the observer's admission rule was not weakened.

The alternative [passive reader](ReadAioRequest.py) uses only process query and memory-read access. It does not attach a debugger, suspend threads, execute remote calls, submit GPU work, or write target memory. It pins the physically mapped PD/NR disk images and verifies selected mapped route instructions. The chain interface's actual vtable method must be PD `110E60`; its `+78` backend must equal the backend getter's pointer. Those identities are checked before and after each pair of packet reads. Matching reads are a consistency check, **not an atomic snapshot**.

Independent decoding verifies that the chain copies its `0x598` v1 request into backend `+3240` (`A5FD0..A62DF`). The pass table starts at packet `17C`, stride `68`; pass ID and Style are distinct fields. Scalar loads and the Style load are verified in `A37F1..A3850`. Count is packet `174`; the API admits up to ten passes. Pass `+24` is retained as `groupCountField`, not the separate saved INI `Customized` value.

The reviewed capture build obtained **15 accepted / 0 rejected snapshots** over approximately 15 seconds. All fifteen raw packets have different hashes; this is not proof of fifteen evaluations. Every snapshot contains one requested pass, enabled, ID 1, **Style 0 / Tone 1**, intensity/structure/skin structure 1, auto skin off, mask groups off, and `groupCountField=4`. The model callback slot was null at each separate pointer read. Skyrim remained responsive after capture. No installed DLL, INI or MO2/profile setting changed.

The source was reviewed independently. Route admission, rejection of all-inconsistent reads and bracketing pointer checks were strengthened before the recorded live run. Subsequent offline decoding corrected the `+24` label and the decoder's count bound; the receipt distinguishes capture-tool and decoder hashes. All recorded packets contain one pass. Proprietary binaries and full packet captures stay local; the public receipt retains selected controls and capture hashes.

## What remains open

These are **retained requested controls**, not the network-entry frame after callback/configuration. Neither freshness, actual evaluation cadence, final controls, complete resets, resource/view formats, guide/image pixels, nor the active viewport has been established. The user's stable AIO Style-0 result remains the positive visual reference. This capture does not fix Dva's camera color drift or advance the **1 of 8** milestone count.

Continue the actual producer/resolve contract investigation using passive state and static route evidence. Prioritize Color/Output formats, transfer, viewport and guides before changing Dva's production color preparation. Do not retry debugger capture on this installation while the attach hook remains unqualified. Do not lower Tone, re-enable the rejected Stable colors option, or introduce a speculative format/Backbuffer patch based only on these stored controls.

## Reproduce the passive request capture

With the supplied pinned AIO19 images active, one Skyrim instance loaded, and Python/pefile installed:

```powershell
C:/Python314/python.exe research/nr/tone-contract/runtime-observer/ReadAioRequest.py --pid <SkyrimPID> --output <existing-directory>/aio-request.json
```

Run at the same privilege level as Skyrim if Windows denies query/read access. The caller creates the output directory. The reader permits 1–30 samples, intervals 100–1000 ms, and defaults to 15 samples at one-second intervals. A capture with no usable packet is inconclusive and returns failure. Read failures preserve available diagnostic output. This tool is AIO-specific; it does not admit Dva's different runtime image.

## Native texture capture prepared

The [Ghidra/Capstone preparation checkpoint](NATIVE_METADATA.md) now extends the
passive reader to qualified native texture dimensions and resource formats.
Eleven owned descriptors match `GetDesc`; five unqualified/inconsistent cases
are refused. The root AIO launcher now uses the passive wrapper. The prior
captures above contain no native texture metadata; their offline route decode
must not be relabeled as a new format capture. Actual Skyrim formats, views,
transfer and pixels remain open.
