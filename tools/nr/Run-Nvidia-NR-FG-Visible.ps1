param([string]$BuildDirectory = '')
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
$log = Join-Path $resultDirectory ('nvidia-vendor-visible-' + (Get-Date -Format 'yyyyMMdd-HHmmss') + '.log')
Write-Host 'Keep Skyrim closed. Click the NVIDIA test window and leave it in the foreground for about 25 seconds.'
Write-Host 'The moving scene resizes once. No Skyrim, MO2 or installed mod files are changed.'
Push-Location $workspace
try {
    # Windows PowerShell treats normal native stderr logging as ErrorRecords.
    # Preserve those messages without terminating the renderer at its first log.
    $savedPreference = $ErrorActionPreference
    $ErrorActionPreference = 'Continue'
    & $exe $nr $core --vendor-visible $runtime 2>&1 | Tee-Object -FilePath $log
    $probeExit = $LASTEXITCODE
} finally { $ErrorActionPreference = $savedPreference; Pop-Location }
Write-Host "Result log: $log"
if ($probeExit -eq 0) { Write-Host 'PASS: actual NVIDIA generated-presentation counters and real/source pixels qualified.' }
elseif ($probeExit -eq 77) { Write-Host 'NOT QUALIFIED: the test window did not stay in the foreground. Run again and click the scene window.' }
else { Write-Host "FAILED: probe exit $probeExit. Keep this log for inspection." }
exit $probeExit
