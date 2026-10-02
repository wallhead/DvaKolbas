# Renderer logs

The log is in `Documents/My Games/Skyrim Special Edition/SKSE/TheosRenderPipeline.log`.
It records date, time, thread, plugin version, edition, source revision, loaded DLL path and Skyrim runtime. The renderer retains its own logger when initializing SKSE, so the startup banner survives.

Opening the logger rotates the previous log. Up to three archives (`TheosRenderPipeline.1.log` through `.3.log`) are retained; files also rotate at 5 MiB. These are recent log segments, not guaranteed complete sessions when verbose logging fills a file.

Normal mode logs startup settings, actual FSR provider and extents, first-frame handoff descriptors and up to 16 spatial/temporal transitions. Frame failures always record the conversion stage/HRESULT, input/output resource ownership and descriptors, rendering counters, device-removal status and one UI/viewport snapshot. Detailed traces are not required to capture a fatal failure.

Optional switches in `SKSE/Plugins/TheosRenderPipeline.ini`:

```ini
[Debug]
LogFrameDiagnostics=false
LogPerformanceMetrics=false
PerformanceLogIntervalSeconds=10
```

`LogFrameDiagnostics=true` enables sampled frame/camera and native UI traces, plus further route transitions. It can produce large logs. `LogPerformanceMetrics=true` enables GPU timing summaries at the configured wall-time interval (clamped to 1–120 seconds). GPU timing must also be enabled under `[Performance]`; HUD measurement and binary frame tracing remain independent of text logging.

For the current black screen, launch Skyrim through the usual MO2 SKSE entry, close the game after the failure, and inspect the current log. The logging update does not establish or repair the underlying frame-delivery cause. Source revision and DLL path identify whether the intended test build actually loaded.

Automated checks cover quiet defaults, INI round trips, interval bounds and production WARP GPU measurements with text logging disabled/enabled. The 2026-10-02 Skyrim launch confirmed startup-banner retention, rotation of the previous log and a detailed automatic failure snapshot. Headless GPU fixtures do not exercise the SKSE startup entry point or establish gameplay acceptance.
