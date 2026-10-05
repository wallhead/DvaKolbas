# Automatic settings and Native render scale

The menu now keeps the frame-time graph above three standard ImGui tabs:
DLSS, NR and Frame generation. The pipeline diagram and its health strip are
removed from the menu. Apply and Discard buttons are removed.

Live edits go through the existing renderer settings controller automatically,
once per edited UI frame. Invalid edits remain visible with an error and leave
the active configuration unchanged. Idle frames do not resubmit rejected or
restart-only requests. Save as default persists choices and window layout;
ordinary edits do not write the INI.

DLSS and FSR are the visible upscaler choices. Both offer `100% | Native` under
Render scale. DLSS Native still uses the existing DLAA implementation and INI
mode ID 3. Scaled DLSS retains mode ID 0 and the existing quality IDs. Provider
switches retain the NVIDIA Native choice and each provider's dependent settings.
Mode, render scale and presenter changes retain their existing restart notices.
This change does not introduce renderer or swapchain hot replacement.

The installed trial INI, runtime payloads and MO2 launch/profile settings are
preserved. Only the plugin DLL and its validation manifest are updated after
Skyrim and MO2 close; the working DLL/INI are backed up first.

## Validation

- Full Release plugin and configured test targets built successfully.
- Selected integrated suite: 259 passed, zero failures or skips, including
  93 GPU and 7 ReShade checks; 270.41 seconds.
- After the final provider-memory and fine-sharpness corrections, the final
  build and eight affected CPU checks passed: BackendSelection,
  WeatherAppearance, OverlayLayout, OverlayNeuralTab, RendererSettingsActions,
  RendererSettingsController, OverlayCommunityNeuralRepair, SourceUpscalerDeferred.
- Regressions exercise automatic NR/FG edits through the production controller,
  no implicit INI saves, rejected edits, 120 idle redraws without resubmission,
  correction of invalid input, Native/scaled mappings and provider round trips.
- A fine sharpness edit regression failed before the exact edit comparison and
  passed after it. Initial new API regressions failed compilation until implemented.
- Independent final review found no remaining P1/P2 issues; diff whitespace check passed.

The six previously excluded environment/foreground checks were not rerun:
NativeUIComposition, NativeUIBlendState, NeuralPeripheralPixels,
NrPostDlssVendorPresentationGpu, NrPostDlssVendorLifecycleGpu,
NrPostDlssVendorInterruptionGpu. No new Skyrim menu observation is claimed.

Local build/test evidence is under `out/research/menu-auto-apply-*.log` and
`out/research/menu-auto-apply-*.xml`. Installation identity, hashes and rollback
location are recorded in the staged package's `menu-update.json` receipt.
