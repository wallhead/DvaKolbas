[CmdletBinding()]
param()
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$exe=Join-Path $root 'out/build/fsr-fg-universal/Release/TRPXessTemporalGpuTests.exe'
$sdk=Join-Path $root 'out/research/xess-sdk-3.0.2/SKSE/Plugins'
$fixture=Join-Path $root 'out/build/fsr-fg-universal/xess-fixtures/good'
if (-not (Test-Path -LiteralPath $exe)) { throw 'Build TRPXessTemporalGpuTests before running the visible scene.' }
$pin=Get-Content -LiteralPath (Join-Path $PSScriptRoot 'sdk-pin.json') -Raw | ConvertFrom-Json
$dll=Join-Path $sdk 'RaZkolbaS/XeSS/libxess.dll'
if ((Get-FileHash -LiteralPath $dll -Algorithm SHA256).Hash.ToLowerInvariant() -ne $pin.runtime.sha256) { throw 'Pinned official XeSS DLL identity differs.' }
$logs=Join-Path $root 'out/validation/xess'
[IO.Directory]::CreateDirectory($logs) | Out-Null
$log=Join-Path $logs ("visible-{0:yyyyMMdd-HHmmss}-{1}.log" -f (Get-Date),[guid]::NewGuid().ToString('N').Substring(0,6))
Write-Output 'Keep Skyrim closed. Watch Native -> Quality -> Performance; report wobble, flashing colours or severe edge shimmer. No Skyrim/MO2 files change.'
& $exe $sdk $fixture --visible 2>&1 | Tee-Object -FilePath $log
$result=$LASTEXITCODE
Write-Output "Saved: $log"
if ($result -ne 0) { throw "XeSS visible test exited $result; inspect the saved log." }
