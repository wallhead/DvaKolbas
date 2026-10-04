# Effective NR controls observer

This research tool prepares the next actual-game boundary comparison for the
user-confirmed stable AIO19 Style 0 / Tone 1 reference. It is not a drift fix.
The shipping DLL, INIs, MO2 launch settings and profile are unchanged.

The external Windows x64 debugger observes the exact pinned NR runtime's network
entry at RVA `0x21BB0`, reading its third argument (`R8`) as the parsed frame.
That point follows callback/configuration processing. Both admitted disk images
have the same 16-byte instruction prefix there. Hash admission covers the
original AIO19 image and the installed RTX40 community image only. Unknown,
duplicate or absent NR modules are refused. Full mapped-image identity beyond
the observed prefix is not asserted.

One execution hardware breakpoint is installed per debugged thread, including
new threads. The tool saves/restores debug registers, refuses existing enabled
hardware breakpoints, passes unrelated exceptions through, bounds capture to
30 seconds / 64 samples, disables debugger kill-on-exit and drains already-raised
owned breakpoint exceptions before detaching. No target instructions/data are
patched, no DLL is injected, no GPU work is submitted and no target process is
terminated. The cancel file requests early cleanup and capture otherwise stops
automatically. The executable has a console cancellation handler, but terminal
hosts may terminate a native command themselves; only file cancellation has
been qualified. Forcibly terminating the observer is not a qualified path.
The system attach breakpoint is identified by its first-chance status, verified
remote `ntdll!DbgBreakPoint` address and the thread starting at verified
`ntdll!DbgUiRemoteBreakin`; an arbitrary first target breakpoint is not consumed.

## What the samples establish

- Nine resource pointers and corresponding dispatch subrects (Color, motion,
  depth, Output, ControlMask, UI, UIAlpha, Backbuffer, distortion).
- Effective Style, Tone, intensity, structure, requested/effective skin
  structure, auto mask, reset, enabled, depth inversion and UI correction.
- Motion scaling, callback pointer presence and the indexed 14-float
  configuration block. These coefficients have no assigned semantic names.

Frame offsets are recovered from the pinned vendor parser, not PD's separate
input packet: resource records have stride `0x18`; MV scale is `D8/DC`,
intensity/Tone/structure is `E0/E4/E8`, Style is `EC`, auto-mask is `F0`,
requested/effective skin structure is `F4/F8`, reset is `100`, depth inversion
is `104`, enabled is `108`, UI correction is `10C`, axis inversion is `110/114`,
and coefficients begin at `124`.

**Still missing:** resource/view formats, actual Color/Output/guide pixels and
active source viewport. Dispatch subrects alone do not prove that viewport.
Samples do not establish a complete reset history. Debugged runs are not
performance measurements. No actual Skyrim attachment has been qualified yet.

## Build and checks

From this worktree, with Windows x64/MSVC and Python available:

```powershell
cmake -S research/nr/tone-contract/runtime-observer -B out/build/nr-boundary-observer -A x64
cmake --build out/build/nr-boundary-observer --config Release
ctest --test-dir out/build/nr-boundary-observer -C Release --output-on-failure
```

[Owned CPU qualification](cpu-validation.json) records **11 cases repeated ten
times**. The fixture tests real attach/capture/detach, new threads, active
cancellation, timeout, read failure, target exit, partial arming with an existing
breakpoint, foreign breakpoint handling and unknown-image/executable refusal.
The foreign-breakpoint case does not deterministically reproduce the reviewed
initial ownership race. The regression caught queued
single-step exceptions that could otherwise fault a worker after detach.

[Owned GPU qualification](owned-validation.json) covers both exact NR models,
32 effective snapshots each, and baseline versus observed runs with 160
evaluations / one reset each. Their CSVs and sixteen RGB image files match;
all source alpha pixels were checked by each run. This qualifies the observer
on those standalone fixtures, not on the actual game or as a timing tool.
The first-source gate derivative and GPU outputs remain under the receipt's
local `out/` directory; the earlier fixture source/receipts were preserved.

## Actual-game procedure

1. Use V5.4 NO-LORE with AIO19 selected and Dva disabled. Start Skyrim normally,
   load the building scene, enable NR and select Style 0 / Tone 1; leave FG off.
   The last AIO toggle comparison saved NR **off**, so enable it in the game.
2. Open `Capture-AIO19-NR.cmd` in this worktree. Rotate the camera around the
   same building for roughly 10–15 seconds. Wait for `FINISHED`.
3. The capture and selected before/after settings are written under
   `out/research/nr/tone-boundary-game/`. Authentication sections are omitted.
   The script verifies host/model hashes and reads the existing profile; it
   does not change any mod settings or launch the game.
4. A later Dva capture uses `Capture-Dva-NR.cmd` after selecting only Dva and
   matching the same scene/Style 0/Tone 1/FG-off controls. Compare effective
   controls first before adding new format/pixel instrumentation.

Win32 lifecycle references: [attach behavior](https://learn.microsoft.com/en-us/windows/win32/api/debugapi/nf-debugapi-debugactiveprocess),
[kill-on-exit policy](https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-debugsetprocesskillonexit),
[detach](https://learn.microsoft.com/en-us/windows/win32/api/debugapi/nf-debugapi-debugactiveprocessstop),
[debug events and handle lifetime](https://learn.microsoft.com/en-us/windows/win32/api/debugapi/nf-debugapi-waitfordebugevent).
