param(
    [Parameter(Mandatory)][string]$BuildDirectory,
    [Parameter(Mandatory)][string]$RuntimeDirectory,
    [Parameter(Mandatory)][string]$StagingDirectory,
    [Parameter(Mandatory)][string]$OutputArchive,
    [string]$SevenZip='C:/Program Files/7-Zip/7z.exe'
)
$ErrorActionPreference='Stop'
. (Join-Path $PSScriptRoot '../nr/RuntimePackageCommon.ps1')
Add-Type -AssemblyName System.IO.Compression.FileSystem
$stage=[IO.Path]::GetFullPath($StagingDirectory);$archive=[IO.Path]::GetFullPath($OutputArchive)
if((Test-Path -LiteralPath $stage) -or (Test-Path -LiteralPath $archive)){throw 'Validation output already exists'}
$release=Join-Path $BuildDirectory 'Release'
$identity=Get-EmbeddedBuildIdentity (Join-Path $release 'RaZkolbaS.dll')
if(-not $identity.sourceClean -or -not $identity.fsrCompiled -or -not $identity.frameGenerationCompiled){throw 'Clean FSR/FG build required'}
$runtime=Join-Path $stage 'plugins/FSR'
[IO.Directory]::CreateDirectory($runtime) | Out-Null
$sources=Join-Path $stage 'sources';[IO.Directory]::CreateDirectory($sources) | Out-Null
$pins=@((Get-Content (Join-Path $PSScriptRoot 'runtime-pin.json') -Raw | ConvertFrom-Json).runtime)+@((Get-Content (Join-Path $PSScriptRoot 'fg-runtime-pin.json') -Raw | ConvertFrom-Json).runtime)
foreach($pin in $pins){Assert-PinnedFile (Join-Path $RuntimeDirectory $pin.filename) $pin.sha256 $pin.bytes;Copy-Item -LiteralPath (Join-Path $RuntimeDirectory $pin.filename) -Destination $runtime}
Copy-Item -LiteralPath (Join-Path $release 'TRPFsrGenerationProviderProbe.exe'),(Join-Path $BuildDirectory 'tests/fsr-fg/Release/TRPFsrGenerationGpuSmoke.exe') -Destination $stage
Copy-Item -LiteralPath (Join-Path $PSScriptRoot '../../tests/fsr-fg/FSRGenerationGpuSmoke.cpp'),(Join-Path $PSScriptRoot '../../tests/fsr-fg/PresentationObserver.h') -Destination $sources
Copy-Item -LiteralPath (Join-Path $PSScriptRoot 'Test-Ml-FrameGeneration.ps1') -Destination (Join-Path $stage 'Test-FSR4-FG.ps1')
[IO.File]::WriteAllLines((Join-Path $stage 'Start-FSR4-FG-Check.cmd'),[string[]]@('@echo off','powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0Test-FSR4-FG.ps1"','if errorlevel 1 echo FAILED - send the results folder.','pause'),[Text.Encoding]::ASCII)
$files=@(Get-ChildItem -LiteralPath $stage -Recurse -File | ForEach-Object {
    [ordered]@{path=[IO.Path]::GetRelativePath($stage,$_.FullName).Replace('\','/');bytes=$_.Length;sha256=(Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant()}
})
[ordered]@{sourceRevision=$identity.sourceRevision;expectedFgProvider='4.0.1';qualification='pending AMD GPU';order='DLSS/FSR -> NR -> FG -> UI';files=$files} |
    ConvertTo-Json -Depth 6 | Set-Content -LiteralPath (Join-Path $stage 'manifest.json') -Encoding utf8
Push-Location -LiteralPath $stage
try {& $SevenZip a -tzip $archive '*' -mx=5 -bd | Out-Null;if($LASTEXITCODE -ne 0){throw 'Kit archive creation failed'}}
finally {Pop-Location}
& $SevenZip t $archive -bd | Out-Null;if($LASTEXITCODE -ne 0){throw 'Kit CRC validation failed'}
$packed=[IO.Compression.ZipFile]::OpenRead($archive)
try {
    $expected=@{};foreach($file in Get-ChildItem -LiteralPath $stage -Recurse -File){$expected[[IO.Path]::GetRelativePath($stage,$file.FullName).Replace('\','/')]=(Get-FileHash -LiteralPath $file.FullName -Algorithm SHA256).Hash.ToLowerInvariant()}
    $count=0
    foreach($entry in $packed.Entries){
        if($entry.FullName.EndsWith('/')){continue}
        $hash=[Security.Cryptography.SHA256]::Create();$stream=$entry.Open()
        try {$actual=[BitConverter]::ToString($hash.ComputeHash($stream)).Replace('-','').ToLowerInvariant()}
        finally {$stream.Dispose();$hash.Dispose()}
        if(-not $expected.ContainsKey($entry.FullName) -or $actual -ne $expected[$entry.FullName]){throw "Kit packed content differs: $($entry.FullName)"}
        ++$count
    }
    if($count -ne $expected.Count){throw 'Kit packed inventory differs'}
} finally {$packed.Dispose()}
Write-Output ('PASS: portable FSR4 FG validation kit packed and verified; SHA256='+ (Get-FileHash -LiteralPath $archive -Algorithm SHA256).Hash.ToLowerInvariant())
