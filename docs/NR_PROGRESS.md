# NR implementation progress — 2026-10-03

**1 of 8 milestones complete:** Task 2, the standalone direct runtime catalog/owner and RTX 40 GPU qualification. Task order differs from completion count: Task 1 true post-FG feasibility remains open.

| Task | Status | Evidence or next requirement |
| --- | --- | --- |
| 1. Post-FG output and matching generated guides | In progress | Independent reference renderer/schema tests; FSR color/UI/retirement baseline passes. NVIDIA static output candidates identified; generated guides remain unqualified. |
| 2. Catalog, direct owner and compatibility loading | Complete for standalone scope | Exact 65-ID catalog, locked-file hashes, narrow IAT shim; 14/14 NR + legacy contract tests. Reviewed clean-source RTX 40 30-frame RGB readbacks and retired teardown pass. |
| 3. Shared one-pass stage and Before integration | Next | Move new parameter policy and legacy compatibility adapters here, together with the real game/shared-stage call sites. Old game profiles remain untouched until then. |
| 4. FSR true After | Pending Task 1 guides | Generated color access is proven; matching generated depth/motion/history is required. |
| 5. DLSS-G true After | Pending Task 1 boundary/guides | Exact installed Streamline 2.13 candidate functions are static research only. |
| 6. Live controls/lifecycle | Pending stage integration | One authoritative setting, frozen batch and retained runtime identity. |
| 7. Clean matrix/packages/final review | Pending implementation | Current checkpoint review is narrower than the future whole-branch review. |
| 8. Separate Skyrim acceptance | Pending package and user | Accepted FSR mod and MO2 settings remain unchanged. |

RTX 20/30/50 hardware is NOT RUN. AMD is explicitly unsupported. Current qualified driver core is pinned; other cores are unqualified. A successful catalog selection is not a claim of GPU output. Details and bounded receipts: [runtime checkpoint](../research/nr/runtime-catalog/README.md).

Checkpoint review found three issues and all were fixed: CNG backing storage lifetime; RGBA hashes admitting alpha-only changes; and partial RGB writes admitting untouched channels. Two output regressions were observed RED→GREEN. The buffer-lifetime finding follows the CNG API ownership requirement; no deterministic runtime RED was claimed. The final product-tree NR/legacy contract run passed 14/14, without skips.
