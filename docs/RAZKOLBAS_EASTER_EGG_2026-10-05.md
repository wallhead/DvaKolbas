# razkolbas audio tab

The fourth standard ImGui tab is named `razkolbas`. It contains the supplied
Kolbasny tsekh recording, Play and Stop buttons, and playback/error status.
There is no autoplay. Play starts at the beginning; Stop resets playback.
Closing the menu or selecting another tab leaves playback running.
The existing frame graph and renderer controls are retained.

The MP3 is packaged as
`SKSE/Plugins/TheosRenderPipeline/Audio/razkolbas.mp3`.
Reading, Windows audio initialization, decoding and playback run on one lazy
worker. Present only submits commands and reads status. Reading into a memory
stream through the game's filesystem hooks supports MO2's virtual Data tree
without requiring a Windows file broker to resolve that tree. No renderer
device, swapchain, settings or Steam hooks are changed.

Commands retain only the latest request. Stale opens cannot overwrite a newer
request's status. Media failures and natural completion are reported. Stop
closes the player/source/stream; the next Play opens the track from the start.
Audio errors stay in the optional tab and do not stop the game renderer.

The game owner retains the lazy worker until process exit, avoiding a thread
join under the DLL loader lock. Standalone owners explicitly stop/join and
close Windows media objects on the worker thread.

Validation includes muted decoding of the exact supplied MP3, missing/corrupt
files, nonblocking submission, stop/restart, rapid commands, natural completion,
active-owner destruction, and compact menu layout. In-game audible playback
and MO2 virtual-file resolution require the owner's next Skyrim check.

The broader Standard non-GPU/non-interactive run passed 168 of 171 checks.
NativeUIComposition, NativeUIBlendState and NeuralPeripheralPixels could not
initialize the Windows graphics debug layer (`0x887A002D`); neither
`d3d11sdklayers.dll` nor `dxgidebug.dll` is installed in System32.
These unchanged standalone graphics tests do not link the new audio worker.
Hardware/interactive renderer suites were not repeated for this audio/menu change.
