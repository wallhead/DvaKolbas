[CmdletBinding()]
param([Parameter(Mandatory)][string]$SourceSdk, [Parameter(Mandatory)][string]$ArchivePath,
      [Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference = 'Stop'
$repo = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$script = Join-Path $repo 'tools/fsr/Acquire-Runtime.ps1'
$pin = Get-Content (Join-Path $repo 'tools/fsr/runtime-pin.json') -Raw | ConvertFrom-Json
$fgPin = Get-Content (Join-Path $repo 'tools/fsr/fg-runtime-pin.json') -Raw | ConvertFrom-Json
$root = Join-Path ([IO.Path]::GetFullPath($OutputRoot)) ([guid]::NewGuid().ToString('N'))
foreach ($header in @($pin.headers) + @($fgPin.headers)) {
    $target = Join-Path $root $header.path
    [IO.Directory]::CreateDirectory([IO.Path]::GetDirectoryName($target)) | Out-Null
    Copy-Item -LiteralPath (Join-Path $SourceSdk $header.path) -Destination $target
}
& $script -Destination $root -ArchivePath $ArchivePath
if (-not $?) { throw 'Default SR acquisition failed' }
$receipt = Get-Content (Join-Path $root 'acquisition-receipt.json') -Raw | ConvertFrom-Json
if ($receipt.runtime.Count -ne 2 -or $receipt.frameGenerationRequested -or
    (Test-Path (Join-Path $root 'runtime/amd_fidelityfx_framegeneration_dx12.dll'))) { throw 'Default SR acquisition loaded/staged FG' }
& $script -Destination $root -ArchivePath $ArchivePath -IncludeFrameGeneration
if (-not $?) { throw 'Opt-in FG acquisition failed' }
$receipt = Get-Content (Join-Path $root 'acquisition-receipt.json') -Raw | ConvertFrom-Json
if ($receipt.runtime.Count -ne 3 -or -not $receipt.frameGenerationRequested) { throw 'FG receipt omitted explicit module request' }
$fgFile = Join-Path $root 'runtime/amd_fidelityfx_framegeneration_dx12.dll'
if ((Get-FileHash $fgFile).Hash.ToLowerInvariant() -ne $fgPin.runtime[0].sha256) { throw 'FG acquisition did not retain pinned bytes' }
$stream = [IO.File]::OpenWrite($fgFile)
try { $stream.Position = 128; $stream.WriteByte(0xff) } finally { $stream.Dispose() }
$rejected = $false
try { & $script -Destination $root -ArchivePath $ArchivePath -IncludeFrameGeneration } catch {
    if ($_.Exception.Message -notlike 'SHA-256 mismatch:*') { throw }; $rejected = $true
}
if (-not $rejected) { throw 'Tampered FG module was accepted' }
Write-Output 'PASS: SR-only acquisition, explicit FG opt-in, pinned receipt, tampered FG rejection'
