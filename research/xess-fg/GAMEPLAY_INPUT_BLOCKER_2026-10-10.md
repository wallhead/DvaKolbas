# XeSS FG gameplay input boundary remains unqualified

## Observed game result

The installed `dd89f752e732` trial loaded on Skyrim 1.6.1170.0. Its first world
hook diagnostic showed 14,734 pre-input callbacks, 14,734 completed input callbacks
and 14,735 render callbacks accumulated since startup. Both installed call sites
were intact. However, all 512 bounded world trace events were render boundaries;
none were pre-input or completed-input boundaries.

The engine source stayed at 20,965 while the rendered frame counter advanced.
The final periodic report at 18:30:25 Moscow time had source 58,800, temporal
evaluation successful, FG requested, `frames=1`, `sdk=0` and `orderedSources=0`.
No report contained `frames=2`. This is not a successful in-game FG test.

## Instruction-flow evidence

The existing read-only instruction capture of update function 36564 contains
`test r15b,r15b` at RVA 0x646390 followed by a conditional jump to 0x6464B7.
That jump skips the direct input poll at 0x646407 (36564 + 0x567), along with
the surrounding input processing. Earlier instructions establish this branch
flag from menu/game state and Main::freezeTime. Normal gameplay can therefore
reach the later render call without passing through the pinned input call.

The previous correction removed reliance on an unused outer update caller,
but still chose a conditional input path. Its adapter-order and timing-owner
tests pass for calls that reach that adapter; they do not qualify its coverage
of the actual gameplay branch. Runtime call counts and instruction witnesses
must both be used when choosing the replacement.

## Loaded-save capture and diagnostic probe

The subsequent loaded-save read-only capture succeeded. Relocations 16405,
36577 and 36567 were inspected; their object iteration, timer bookkeeping and
auxiliary dispatch do not establish input sampling. The selected menu input
site remains intact and its callee entry is detoured by DevBench. Main::Update
has a CBPC entry detour whose retained trampoline restores the original
prologue and returns to the native body. These existing chains must survive.

Extending the mapped-code scan from E8 calls to E9 tail jumps found relocation
36578, RVA 0x647410, with a tail jump at 0x647433 into PollInputDevices (68617).
Its full 40-byte function restores its stack before the tail transfer and sets
RCX to the input manager and XMM1 to the frame interval. The compact evidence
is `gameplay-input-job-profile-1170.json`. Instructions alone do not establish
its scheduling, thread ownership or one-to-one association with raster frames.

The diagnostic build wraps this verified tail jump, preserving the existing
PollInputDevices entry chain. Atomic counters record calls started/completed,
QPC times and thread IDs. The main render callback logs 128 bounded world
snapshots alongside the raster counter and render-thread ID. These independent
snapshots are advisory: concurrent calls cannot be paired from them alone.
The probe does not emit XeLL markers, source IDs or FG admission. All three
instruction witnesses are checked before any installation.

The new CPU regression failed on the original menu-only preflight and passes
with the gameplay witness required. It also checks the tail target, stack
restoration, exactly-once preserved polling and absence of timing callbacks.
Release compilation and five selected CPU/package checks passed; independent
review found no important installation blocker. Physical GPU and gameplay
timing validation are still pending. The existing timing contract is unchanged.

Keep strict same-source, epoch and thread checks. Do not repair this by emitting
the missing markers at Present or dropping the genuine-input requirement.
Milestone progress remains 3 of 8. The next necessary action is installing the
diagnostic DLL with Skyrim/MO2 closed, then observing a new loaded-save run.
