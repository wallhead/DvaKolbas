# Compact menu and visible Save action

The single-column menu defaults to 640 pixels wide and permits resizing to
480 x 420. Smaller displays still take precedence. Existing saved geometry
is retained; resize the window and use Save as default to persist a new size.

The tab body reserves a footer independently of its contents. Each tab's settings
child fills the available width and height after the tab header. Its scrollbar
follows resizing. Save as default sits outside the tab and status scroll regions;
long status messages scroll below the button. Hovering Save explains persistence
and edit completion. Frame-time measurements and all three tabs are retained.

The prior layout calculated a settings height before drawing the tab header and
underestimated footer content. This could push the action button outside the
window. The new body/footer separation removes that calculation.

Validation: Release plugin build succeeded; all eight affected checks passed
with zero failures or skips. The actual ImGui layout regression exercises every
tab at 480 x 420, 640 x 720 and 960 x 900, shrinking back after expansion, long
status text, independent scrolling, parent scrollbar absence and Save bounds.
The compact-default regression failed before the fix. Independent read-only
review found no P1/P2 issues. Evidence is in `out/research/compact-menu-*.log`
and `compact-menu-tests.xml`.

This is a UI layout change; renderer/GPU ownership and runtime settings remain
unchanged. No new Skyrim observation or GPU qualification is claimed. Installation
updates the backed-up plugin DLL and validation manifest with Skyrim closed,
preserving INI, runtime payloads and MO2 launch/profile files byte for byte.
