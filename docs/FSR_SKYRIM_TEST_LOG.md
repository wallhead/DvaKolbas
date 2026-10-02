# Skyrim FSR SR test log

## 2026-10-02: first launch, buffer-cache failure

Target: TESV54BETA / V5.4, profile `V5.4 NO-LORE`. A separate `TRP FSR SR Standard - DvaKolbas test` mod was enabled, with the working DLSS baseline retained and disabled. The user's MO2 settings and launch entry were preserved. Startup requests FSR Quality, Analytical provider, sharpness zero, ordinary backend 0, native UI enabled, and FG/NR/HDR/dynamic resolution disabled. Gamma22 remains provisional and requires Skyrim/ENB color calibration.

The user launched Skyrim. The 10:55:58–10:56:37 log confirms `UpscaleType=4` and interpolation disabled, then reports `[CoreHost] inner buffer cache failed result=0x887A0001`. The fatal dialog incorrectly labeled this as a NVIDIA DLSS-G game-facing buffer failure. No temporal FSR context or loaded gameplay was reached.

Root cause: the host tried to cache all presentation buffers through `GetBuffer(index)` using the NVIDIA transport's independently indexed resource contract. The ordinary D3D11 flip-discard swapchain exposes its current renderable buffer through `GetBuffer(0)`. Native D3D11 rotates buffer identities after Present; it does not require the D3D12-style physical-index selection. See [Microsoft's DXGI overview](https://learn.microsoft.com/en-us/windows/win32/direct3ddxgi/d3d10-graphics-programming-guide-dxgi).

The production startup helper was extended to call the actual host presentation cache. Before the fix, it reproduced the exact `0x887A0001` error. The correction gives ordinary FSR an explicit D3D11-current-buffer cache and maps reconstruction, Community Shaders presentation and both native UI selection paths to that current buffer. NVIDIA's indexed transport contract is retained. The fatal buffer-creation label now identifies FSR when FSR is active.

The updated helper passes: it caches the current native D3D11 buffer, checks logical selection for both physical indices, delivers 12 temporal FSR frames with 11 changing native readbacks, checks a native UI pixel sentinel on every frame, releases buffer/view references before resize and recreates the presentation cache and UI target at the new extent. The helper window is hidden; this is a cache/selection/output/resize check, not visual presentation or Skyrim gameplay acceptance. Standard Release passed all 118 runnable tests, including the 1000-frame GPU run and actual ReShade presentation fixtures. An FSR-disabled Standard plugin also rebuilt successfully and passed all 108 runnable tests (226 total across the two configurations). The three unavailable Graphics Tools checks remain excluded.

The corrected Standard DLL from clean code revision `3f463fdfc6fc68eaecb80cb37cbdc7fb4af39a40` is installed in the existing test mod. Its SHA-256 is `730525e8d43b41e08bb35fd687a97b0fbae3b537ed12e2b90120b1d9b1c63505`; the installed manifest, exact pinned runtimes, settings and imports passed package validation. The startup INI, ImGui layout and MO2 profile were retained; the failed DLL and log are backed up locally. The fresh Standard archive is `out/packages/2026-10-02/startup-fix/fsr-standard/Standard-FSR-SR.zip`.

**Pending:** retry Skyrim manually. Reaching a loaded world, actual temporal FSR status, game-producer formats/guides, ENB color calibration, image quality, native UI behavior, gameplay lifecycle and performance remain unverified. FSR frame generation remains unimplemented.

## 2026-10-02: retry reaches context creation, first loading frame fails

The user reports a black screen. The 11:28:57–11:32:59 log confirms that the buffer-cache correction worked: the host prepares a 1706x960 render target for 2560x1440 output, caches one native D3D11 buffer, completes deferred FSR startup, initializes native UI and captures render-sized depth/motion. The first loading-menu frame at 11:29:37 then reports `FSR frame delivery failed (0x80004005)` and latches the rendering-stop state. No temporal frame or gameplay was delivered.

This is a later failure than the previous swapchain-cache error. Loading/menu evaluation uses `FsrFrameAdapter::Spatial`, but that path returned errors without recording `LastError`; the host consequently logged the old context-ready status instead of the actual conversion failure. The precise black-screen cause is not yet established.

Diagnostic correction: spatial extent, bridge and conversion failures now retain their own diagnostic and native HRESULT. Color conversion records whether it failed on encoding, resource ownership, descriptors, shader setup, SRV/RTV creation, context isolation or device status. First-frame logging records the real input/output descriptors and context/resource device-identity checks. Rendering still stops on an actual delivery failure; no ownership checks or error conditions were bypassed.

Before the diagnostic correction, a new regression reproduced the missing spatial extent diagnostic. Afterward, extent, missing render binding and bridge-fault diagnostics pass, together with the ordinary FSR startup/pixel/UI/resize helper. A new Skyrim run is required to identify the failing conversion step; the black screen remains unresolved.


The diagnostic Standard build from clean code revision `ceb5c18e333f89a38270f1f5c26d2a7e866624f2` passed all 118 runnable tests and is installed in the same test mod. Installed DLL SHA-256: `fbb585b9c2ff1f729c5341285e45c3fd57f49bdb52d343715989cb38f59d27f2`. Exact package/runtime/import/configuration validation passed. The existing INI and MO2 profile were preserved, and the previous DLL/log were backed up. The next manual launch must capture the failing spatial-conversion stage; this is an instrumented investigation build, not a claim that the black screen is fixed.
