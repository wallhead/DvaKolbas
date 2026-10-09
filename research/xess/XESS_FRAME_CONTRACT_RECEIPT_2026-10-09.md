# XeSS frame contract receipt — 2026-10-09

Milestone 3: pure input/guide/jitter/sizing contract tested. This does not qualify temporal GPU output or Skyrim.

`XessFrameAdapter` regression failed first for the motion guide-size mismatch, then passed after implementation. The adapter rejects unknown source encoding, unconverted colour, mismatched/non-finite motion scales, different depth conventions, non-finite/out-of-range jitter, wrong prepared formats and unknown/dilated low-resolution motion guides. It requires a source ID/epoch and resets for the first epoch, changed epoch, camera cut or explicit reset.

Native maps to SDK AA (106), Quality/Balanced/Performance to 103/102/101. Input dimensions come from the live SDK context query and are checked against its returned bounds. A boundary fixture returns 1506×848 for 2560×1440 to detect any substitution of FSR ratios.

## Jitter conventions

The actual RaZkolbaS hook in `src/UpscalerHooks.cpp` sets engine jitter to `-2*x/W,+2*y/H` and stores sample offsets `-x,-y`. This matches the traced AIO19 hook and the SDK projection relationship `+2*sampleX/W,-2*sampleY/H`. The adapter forwards stored sample offsets unchanged; it never negates them a second time.

Tests use independently calculated `.25,-.125` at 1600×900: SDK sample `-.25,.125`, projection `-.0003125,-.0002777778`. Additional checks cover the first Halton sample and repetition. Sequence length is `ceil(8*(outputWidth/inputWidth)^2)`, from the pinned official guide's jitter-sequence formula: Native uses 8; 2560/1000 uses 53. It is independent of FSR's jitter query.

## Guide provenance gate

The existing game capture supplies the original source-sized signed RG16_FLOAT guide; the plugin copies it without a dilation operation. That establishes the plugin's processing, not every behavior of the game shader. The new XeSS policy therefore requires **explicit undilated provenance** and rejects Unknown/Dilated. The standalone scene will generate known undilated guides. Game-side qualification must confirm the captured guide's contents before binding it in Task 5.

The pure policy carries the prior source epoch supplied by its context owner. It owns no GPU texture or history resource; SDK and resource lifetime are Task 4 responsibilities.
