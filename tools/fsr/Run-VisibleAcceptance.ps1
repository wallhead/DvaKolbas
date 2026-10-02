param([string]$BuildDirectory,[string]$RuntimeDirectory,[string]$OutputDirectory)
$ErrorActionPreference='Stop'
$repository=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
if(-not $BuildDirectory){$BuildDirectory=Join-Path $repository 'out/build/fsr-fg-standard'}
if(-not $RuntimeDirectory){$RuntimeDirectory=Join-Path $repository 'out/research/sdk-v2.3.0/runtime'}
if(-not $OutputDirectory){$OutputDirectory=Join-Path $repository 'out/research/fsr-fg-visible'}
$fixture=Join-Path $BuildDirectory 'tests/fsr-fg/Release/TRPFsrGenerationGpuSmoke.exe'
if(-not (Test-Path -LiteralPath $fixture)){throw 'Build TRPFsrGenerationGpuSmoke first'}
$run=Join-Path ([IO.Path]::GetFullPath($OutputDirectory)) ((Get-Date -Format 'yyyyMMdd-HHmmss')+'-'+[guid]::NewGuid().ToString('N'))
[IO.Directory]::CreateDirectory($run)|Out-Null
Write-Output 'Move the window onto a >=144 Hz screen. Observe moving red object, fixed red/half-alpha green HUD and white crosshair.'
Write-Output 'Automatic sequence: 10 seconds FG off, 10 on, 5 menu (off), then on. Space toggles FG; M toggles menu; Esc exits safely.'
Write-Output 'Report whether HUD is intact, FG on is smoother and menu/re-entry stays correct. Native DXGI statistics are diagnostic; callbacks alone do not prove scanout.'
& $fixture --visible true --frames 2880 --recreate 1 --debug auto --runtime ([IO.Path]::GetFullPath($RuntimeDirectory)) --output (Join-Path $run 'visible.json') 2>&1|Tee-Object -FilePath (Join-Path $run 'console.log')
$runExit=$LASTEXITCODE
Write-Output "Visible observation report: $run"
exit $runExit
