param([Parameter(Mandatory=$true)][string]$ArchivePath,
      [Parameter(Mandatory=$true)][string]$ScratchRoot)
$ErrorActionPreference='Stop'
$root=Join-Path ([IO.Path]::GetFullPath($ScratchRoot)) ([guid]::NewGuid().ToString('N'))
[IO.Directory]::CreateDirectory($root) | Out-Null
$acquire=Join-Path $PSScriptRoot '../tools/fsr/Acquire-Runtime.ps1'
$badArchive=Join-Path $root 'bad-archive.zip'
[IO.File]::WriteAllText($badArchive,'incorrect release archive')
$failed=$false
try { & $acquire -Destination (Join-Path $root 'bad-archive-target') -ArchivePath $badArchive }
catch { if($_.Exception.Message -notlike '*SHA-256 mismatch*'){throw}; $failed=$true }
if(-not $failed){throw 'Altered archive accepted'}
$valid=Join-Path $root 'verified'
& $acquire -Destination $valid -ArchivePath $ArchivePath
& $acquire -Destination $valid -ArchivePath $ArchivePath
$header=Join-Path $valid 'Kits/FidelityFX/api/include/ffx_api.h'
[IO.File]::AppendAllText($header,"`n// altered header`n")
$failed=$false
try { & $acquire -Destination $valid -ArchivePath $ArchivePath }
catch { if($_.Exception.Message -notlike '*SHA-256 mismatch*'){throw}; $failed=$true }
if(-not $failed){throw 'Altered existing header accepted'}
Write-Output 'PASS: bad archive rejected, valid acquisition/reuse verified, altered header rejected'
