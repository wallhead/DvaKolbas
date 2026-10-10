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

## Next investigation

Capture the actual gameplay branch and the relevant preserved detours while a
loaded save runs. A bounded read-only helper is prepared locally to inspect
relocations 16405, 36577 and 36567 in addition to the original timing witnesses.
These are candidate callees, not established input boundaries. Identify their
meaning and ordering before changing admission or adding a hook.

Keep strict same-source, epoch and thread checks. Do not repair this by emitting
the missing markers at Present or dropping the genuine-input requirement.
Skyrim had already closed when the external capture was attempted; that attempt
returned Windows error 87 and produced no new instruction evidence.

Milestone progress remains 3 of 8. No replacement DLL was installed for this
investigation; the next necessary action is another loaded-save capture.
