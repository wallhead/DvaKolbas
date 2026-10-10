[CmdletBinding()]
param([switch]$NumericalOnly)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$exe=Join-Path $root 'out/build/fsr-fg-universal/Release/TRPXessFgPresentationGpuTests.exe'
$sdkRoot=Join-Path $root 'out/research/xess-sdk-3.0.2'
$sdk=Join-Path $sdkRoot 'SKSE/Plugins'
if (-not (Test-Path -LiteralPath $exe)) { throw 'Build TRPXessFgPresentationGpuTests before running.' }
if (Get-Process SkyrimSE,SkyrimVR -ErrorAction SilentlyContinue) { throw 'Keep Skyrim closed during the standalone GPU test.' }
$pin=Get-Content -LiteralPath (Join-Path $PSScriptRoot 'sdk-pin.json') -Raw | ConvertFrom-Json
$runtimeHashes=@{}
foreach ($runtime in $pin.generationRuntimes) {
    $dll=Join-Path $sdkRoot $runtime.stagePath
    $hash=(Get-FileHash -LiteralPath $dll -Algorithm SHA256).Hash.ToLowerInvariant()
    if ($hash -ne $runtime.sha256) { throw "Pinned official Intel runtime differs: $dll" }
    $runtimeHashes[[IO.Path]::GetFileName($dll)]=$hash
}
$logs=Join-Path $root 'out/validation/xess-fg'
[IO.Directory]::CreateDirectory($logs) | Out-Null
$stem='intel-fg-{0:yyyyMMdd-HHmmss}-{1}' -f (Get-Date),[guid]::NewGuid().ToString('N').Substring(0,6)
$log=Join-Path $logs ($stem+'.log')
$receipt=Join-Path $logs ($stem+'.json')
$arguments=@('--plugin-directory',$sdk)
if (-not $NumericalOnly) { $arguments+='--visible' }
Write-Output 'Keep the Intel scene in foreground until it closes: FG off -> on -> menu/off -> on. Watch motion, red/green HUD and white crosshair. No Skyrim/MO2 settings change.'
& $exe @arguments 2>&1 | Tee-Object -FilePath $log
$result=$LASTEXITCODE
$text=Get-Content -LiteralPath $log -Raw
$summary=[regex]::Match($text,'SUMMARY qualified=(\d+) visual_requested=(\d+) interrupted=(\d+) lost_focus_frames=(\d+) active_sources=(\d+) interpolated_sources=(\d+) off_sources=(\d+) errors=(\d+) clean_retirement=(\d+)')
$qualified=$result -eq 0 -and $summary.Success -and $summary.Groups[1].Value -eq '1'
$data=[ordered]@{
    timestamp=(Get-Date).ToString('o'); qualified=$qualified; exitCode=$result
    sourceRevision=(& git -C $root rev-parse HEAD); executableSHA256=(Get-FileHash -LiteralPath $exe -Algorithm SHA256).Hash.ToLowerInvariant()
    worktreeDirty=[bool](& git -C $root status --porcelain)
    sdkBundle=$pin.release; sdkCommit=$pin.commit; runtimeHashes=$runtimeHashes
    numericalOnly=[bool]$NumericalOnly; summary=$summary.Value; log=$log
    visualUserConfirmation='pending'; debugLayers='see log; unavailable layers do not qualify validation'
}
$data | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath $receipt -Encoding utf8
Write-Output "Saved log: $log"
Write-Output "Saved receipt: $receipt"
if (-not $qualified) { Write-Output 'NOT QUALIFIED. Inspect the log; do not count nominal 2x or an interrupted run as actual FG.'; exit 2 }
Write-Output 'PASS: numerical Intel SDK generation and cleanup. Visual confirmation is separate.'
