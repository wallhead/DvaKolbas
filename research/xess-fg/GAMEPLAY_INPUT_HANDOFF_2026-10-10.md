# Native gameplay input handoff

Status: candidate implementation; Skyrim interpolation is **not qualified**. Milestones remain 3 of 8.

Update: the loaded gameplay run rejected every source because native input did not satisfy the post-Present publication window. The user approved [presentation pacing](PRESENTATION_PACING_2026-10-10.md) instead; the handoff below is historical and no longer a production FG admission requirement.

## Observed failure

The clean `4c6efe59a725` diagnostic recorded 128 bounded render snapshots:

- Render owner: thread 64020.
- Native input workers: 5244, 18644, 28440, 42836, 60752, 65320.
- Raster range: 19671–19798. Input counters advanced once between adjacent snapshots; 127 snapshots contained completed jobs preceding the render timestamp.
- Independent counters do not prove exact source/epoch pairing. The installed menu-only marker path remained inactive in gameplay, and actual output remained one frame per source with zero ordered sources.

These observations explain why moving SDK calls to the menu input hook did not fix gameplay. Intel's timing owner requires SDK calls on its owner thread; the native input job runs elsewhere.

## Candidate implementation

The previously captured 40-byte native job at relocation 36578 is now wrapped at its entry. This includes its first bookkeeping call and the final input poll, avoiding a tail-only observation that could miss earlier input work. The displaced eleven-byte prologue is relocated with its original RIP-relative singleton address and resumes at the original first call. The native body, stack restoration and existing PollInputDevices detour chain remain intact. Other instruction profiles fail qualification.

After a genuine world temporal Present, the render owner reserves the next source's SDK ID and executes XeLL Sleep/SimulationStart. Only after Sleep returns does it publish a source/epoch ticket. The whole native job records its start and actual return through a CPU coordinator; workers never call Intel APIs or inspect engine containers.

Before render, the owner consumes exactly one matching completed native job. Before final native publication, it seals that input proof again; late workers, foreign cancellation, overlap and epoch mismatch reject SDK tags and retain real output. Jobs starting after the completed-render cut remain unarmed future jobs and prevent the next reservation from borrowing an in-flight job. No coordinator mutex spans an input poll, Intel call or GPU wait.

Harmless repeated output preserves the existing next-source reservation. Menu, recovery, reset duplicates, failed output, resize and minimize invalidate it. Foreign threads cannot reserve SDK IDs. Untagged rejected cycles recover through the existing monotonic-ID abandonment path.

The AIO19 reference independently shows next-ID Sleep/SimulationStart after Present (`0x874e7/0x874fd`). That is a candidate schedule, not proof of Skyrim input order. Our handoff must observe real native input after publication before interpolation can qualify.

## Verification and remaining work

CPU tests cover unarmed and early input, overlapping workers, same-thread completion, exact source/epoch, cancellation, late invalidation, sealing, stale tickets, harmless duplicates and recovery. The relocated predecessor is also executed against a synthetic singleton slot and continuation to check actual RIP load and stack balance without using Skyrim or a GPU.

The engine-timing fixture verifies that worker completion adds no SDK calls and the owner retains one ID through all genuine markers. The GPU host fixture verifies that invalid final input proof suppresses resource tags, constants and Present ID even after render markers succeeded.

GPU tests and installation require Skyrim/MO2 closed. Acceptance still requires a new gameplay trace with 128 compatible ordered sources, Intel status reporting two presented frames when FG is requested, one-frame off/menu output, stable scene/HUD, and interruption recovery. If the native job precedes post-Present publication, admission stays inactive; a different independently observed scheduling boundary will be needed. No fallback manufactured input completion is permitted.
