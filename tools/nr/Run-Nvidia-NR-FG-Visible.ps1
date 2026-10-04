param([string]$BuildDirectory = '', [switch]$Lifecycle)
$ErrorActionPreference = 'Stop'
$workspace = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '../..')).Path
if (!$BuildDirectory) { $BuildDirectory = Join-Path $workspace 'out/build/fsr-fg-standard' }
$build = (Resolve-Path -LiteralPath $BuildDirectory).Path
$cache = Get-Content -LiteralPath (Join-Path $build 'CMakeCache.txt')
function CacheValue([string]$key) {
    $line = $cache | Where-Object { $_ -like "${key}:FILEPATH=*" }
    if (@($line).Count -ne 1) { throw "Missing unique configured value: $key" }
    return $line.Split('=', 2)[1]
}
$nr = CacheValue 'TRP_NR_RTX40_DLL'
$core = CacheValue 'TRP_NR_DRIVER_CORE'
$exe = Join-Path $build 'tests/nr-postsr/Release/TRPNrPostDlssSourceGpuTests.exe'
$runtime = (Resolve-Path -LiteralPath (Join-Path $build 'tests/nr-postsr/actual-streamline')).Path
$resultDirectory = Join-Path $workspace 'out/research/nr/post-sr'
New-Item -ItemType Directory -Path $resultDirectory -Force | Out-Null
$prefix = if ($Lifecycle) { 'nvidia-vendor-lifecycle-visible-' } else { 'nvidia-vendor-visible-' }
$mode = if ($Lifecycle) { '--vendor-lifecycle-visible' } else { '--vendor-visible' }
$log = Join-Path $resultDirectory ($prefix + (Get-Date -Format 'yyyyMMdd-HHmmss') + '.log')
Write-Host 'Keep Skyrim closed. Click the NVIDIA test window and leave it in the foreground for about 25 seconds.'
Write-Host 'The moving scene resizes once. No Skyrim, MO2 or installed mod files are changed.'
if ($Lifecycle) {
    Write-Host 'Lifecycle mode runs TWO windows in sequence: a missing-gate negative control, then the actual pending-reader test. Keep each scene window in the foreground; allow about 60 seconds total.'
    Write-Host 'The second run changes tone/style, toggles NR, and pauses briefly while retiring actual NVIDIA readers before feature reentry.'
}
Push-Location $workspace
try {
    # Windows PowerShell treats normal native stderr logging as ErrorRecords.
    # Preserve those messages without terminating the renderer at its first log.
    $savedPreference = $ErrorActionPreference
    $ErrorActionPreference = 'Continue'
    if ($Lifecycle) {
        $controlLog = Join-Path $resultDirectory ('nvidia-vendor-lifecycle-control-' + (Get-Date -Format 'yyyyMMdd-HHmmss') + '.log')
        & $exe $nr $core --vendor-lifecycle-omit-gates-visible $runtime 2>&1 | Tee-Object -FilePath $controlLog | Out-Host
        $controlExit = $LASTEXITCODE
        $controlMarker = Select-String -LiteralPath $controlLog -Pattern '^NVIDIA_VENDOR_LIFECYCLE gates=0 pending=0 reuse=0 reentry=0 omittedGates=1 failures=1$'
        Write-Host "Control log: $controlLog"
        if ($controlExit -eq 77) { Write-Host 'NOT QUALIFIED: control window lost foreground focus. Run again and keep each scene window active.'; exit 77 }
        if ($controlExit -ne 1 -or !$controlMarker) { Write-Host 'FAILED: the negative control did not fail only its missing pending-reader coverage check. Keep this log.'; exit 1 }
        Write-Host 'CONTROL PASSED: missing reader holds were detected. The actual lifecycle window opens next; keep it in the foreground.'
    }
    & $exe $nr $core $mode $runtime 2>&1 | Tee-Object -FilePath $log
    $probeExit = $LASTEXITCODE
} finally { $ErrorActionPreference = $savedPreference; Pop-Location }
Write-Host "Result log: $log"
if ($probeExit -eq 0) {
    if ($Lifecycle) { Write-Host 'PASS: actual NVIDIA pending input-reader settings/reentry checks and source pixels qualified.' }
    else { Write-Host 'PASS: actual NVIDIA generated-presentation counters and real/source pixels qualified.' }
}
elseif ($probeExit -eq 77) { Write-Host 'NOT QUALIFIED: the test window did not stay in the foreground. Run again and click the scene window.' }
else { Write-Host "FAILED: probe exit $probeExit. Keep this log for inspection." }
exit $probeExit
