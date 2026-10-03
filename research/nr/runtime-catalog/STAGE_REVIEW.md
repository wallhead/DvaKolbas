# Shared NR Stage checkpoint review

Scope: native real-source shared Stage, direct retained RuntimeOwner device, packet/history validation, source-alpha restore, submission/readers and probe evidence. Game integration, reconstruction adapters and true post-FG generated images were outside this checkpoint. Review was performed by a fresh read-only agent; it did not edit files or run GPU/game processes.

One important finding: EvaluationTicket used the State address plus a per-stage serial. A retained ticket could match a replacement State allocated at the same address. Fixed by a sealed token retaining a unique identity allocation across owner destruction. `NrTicketOwnership` reproduces exact address reuse using placement construction; observed FAIL before the fix and PASS after it. The same risk in HistoryDecision was independently reproduced by `NrHistory` and fixed using the retained token.

No other confirmed critical or important finding was reported in the stated Stage scope. This does not replace Task 7's future whole-branch review or qualify the later D3D11 preparation/game adapters.

The native Before bridge test initially failed with `E_INVALIDARG` opening an SRV-only FP16 shared texture. The accepted FSR shared-resource descriptor already documents the required RTV capability; adding that same capability made the actual D3D12-to-D3D11 open and 240-frame test pass. It uses native FP16 linear color, R32 depth, RG16 motion, a retained same-adapter immediate context and one pending real-image evaluation. Completion includes the actual D3D11 copy-back reader before resources or features retire. This first bridge waits for completion each frame; game performance and pipelining are not claimed.

An attempted GPU owner-replacement regression with uninitialized tiny input resources failed at device retirement before reaching the identity assertion. It was discarded, not counted as an identity reproduction or product defect. The deterministic CPU identity test supplies the reproducible RED→GREEN result.

Follow-up admission check: native Stage and Before accepted `ResolveMethod::Ratio` without running its required reconstruction. Both actual regressions failed before the fix. They now reject a non-direct/unknown resolve request before vendor work (and, for Before, before input copies). The native bridge's existing 240-frame GPU test and the product-tree NR/legacy contract suite remained green: 21/21, no skips. The later reconstruction adapter must provide the requested math rather than pass those settings directly to the core.
