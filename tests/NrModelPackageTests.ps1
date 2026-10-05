$ErrorActionPreference='Stop'
. (Join-Path $PSScriptRoot '../tools/nr/RuntimePackageCommon.ps1')
function Check([bool]$Value,[string]$Name){if(-not $Value){throw "FAIL: $Name"};Write-Output "PASS: $Name"}
function Rejected([scriptblock]$Action,[string]$Name){$failed=$false;try{& $Action | Out-Null}catch{$failed=$true};Check $failed $Name}
$root=Join-Path ([IO.Path]::GetTempPath()) ('trp-nr-models-'+[guid]::NewGuid().ToString('N'))
[IO.Directory]::CreateDirectory($root)|Out-Null
try {
    $shared=Join-Path $root 'shared.dll';$fp16=Join-Path $root 'fp16.dll'
    [IO.File]::WriteAllBytes($shared,[byte[]](1,2,3,4));[IO.File]::WriteAllBytes($fp16,[byte[]](5,6,7))
    $sharedHash=(Get-FileHash -LiteralPath $shared -Algorithm SHA256).Hash.ToLowerInvariant()
    $fp16Hash=(Get-FileHash -LiteralPath $fp16 -Algorithm SHA256).Hash.ToLowerInvariant()
    $profiles=@(
        [pscustomobject]@{id='rtx50';relativePath='NR/rtx40/nvngx_dlssnr.dll';sha256=$sharedHash;bytes=4;compatibility='CallerIdentityProbeRequired'},
        [pscustomobject]@{id='rtx40';relativePath='NR/rtx40/nvngx_dlssnr.dll';sha256=$sharedHash;bytes=4;compatibility='CallerIdentityProbeRequired'},
        [pscustomobject]@{id='rtx20-30';relativePath='NR/rtx20-30/nvngx_dlssnr.dll';sha256=$fp16Hash;bytes=3;compatibility='CallerIdentityProbeRequired'})
    $sources=@{'NR/rtx40/nvngx_dlssnr.dll'=$shared;'NR/rtx20-30/nvngx_dlssnr.dll'=$fp16}
    $destination=Join-Path $root 'package'
    Copy-NrRuntimeModels -Profiles $profiles -SourcesByPath $sources -RuntimeRoot $destination
    Assert-NrRuntimeModels -Profiles $profiles -RuntimeRoot $destination
    $files=@(Get-ChildItem -LiteralPath $destination -Recurse -File)
    Check ($files.Count -eq 2 -and ($files|Measure-Object Length -Sum).Sum -eq 7) 'ThreeLogicalProfilesCopyTwoPhysicalFiles'
    Check (-not (Test-Path -LiteralPath (Join-Path $destination 'NR/rtx50'))) 'NoSeparateRtx50DirectoryRequired'
    $legacy=Join-Path $root 'accepted'
    $legacyModels=Join-Path $legacy 'SKSE/Plugins/TheosRenderPipeline'
    Copy-NrRuntimeModels $profiles $sources $legacyModels
    $oldFifty=Join-Path $legacyModels 'NR/rtx50/nvngx_dlssnr.dll'
    [IO.Directory]::CreateDirectory([IO.Path]::GetDirectoryName($oldFifty))|Out-Null
    Copy-Item -LiteralPath $fp16 -Destination $oldFifty
    [IO.File]::WriteAllText((Join-Path $legacy 'SKSE/Plugins/TheosRenderPipeline.ini'),'unchanged user settings')
    $trial=Join-Path $root 'trial'
    Copy-NrTrialFiles -Profiles $profiles -SourceRoot $legacy -DestinationRoot $trial
    Assert-NrRuntimeModels $profiles (Join-Path $trial 'SKSE/Plugins/TheosRenderPipeline')
    Check (-not (Test-Path -LiteralPath (Join-Path $trial 'SKSE/Plugins/TheosRenderPipeline/NR/rtx50'))) 'LegacyThirdPayloadOmittedDuringStaging'
    Check ((Get-FileHash -LiteralPath $oldFifty -Algorithm SHA256).Hash.ToLowerInvariant() -eq $fp16Hash) 'AcceptedLegacyPayloadPreserved'
    Check ([IO.File]::ReadAllText((Join-Path $trial 'SKSE/Plugins/TheosRenderPipeline.ini')) -eq 'unchanged user settings') 'StagingPreservesOtherAssets'
    $different=@($profiles|ForEach-Object{$_.PSObject.Copy()});$different[0].sha256=$fp16Hash
    Rejected {Get-NrPhysicalModels $different} 'SharedPathWithDifferentHashRejected'
    $different=@($profiles|ForEach-Object{$_.PSObject.Copy()});$different[0].compatibility='SignedDirect'
    Rejected {Get-NrPhysicalModels $different} 'SharedPathWithDifferentCompatibilityRejected'
    $different=@($profiles|ForEach-Object{$_.PSObject.Copy()});$different[0].bytes=99
    Rejected {Get-NrPhysicalModels $different} 'SharedPathWithDifferentSizeRejected'
    $different=@($profiles|ForEach-Object{$_.PSObject.Copy()});$different[0].relativePath='../nvngx_dlssnr.dll'
    Rejected {Get-NrPhysicalModels $different} 'EscapingModelPathRejected'
    $stale=Join-Path $destination 'NR/rtx50/nvngx_dlssnr.dll'
    [IO.Directory]::CreateDirectory([IO.Path]::GetDirectoryName($stale))|Out-Null
    Copy-Item -LiteralPath $shared -Destination $stale
    Rejected {Assert-NrRuntimeModels $profiles $destination} 'StaleThirdPayloadRejected'
    Remove-Item -LiteralPath $stale
    [IO.File]::WriteAllBytes((Join-Path $destination 'NR/rtx40/nvngx_dlssnr.dll'),[byte[]](9,9,9,9))
    Rejected {Assert-NrRuntimeModels $profiles $destination} 'ReplacedSharedPayloadRejected'
    $missingSources=@{'NR/rtx20-30/nvngx_dlssnr.dll'=$fp16}
    $missingDestination=Join-Path $root 'missing'
    Rejected {Copy-NrRuntimeModels $profiles $missingSources $missingDestination} 'MissingSharedSourceRejectedBeforeCopy'
    Check (-not (Test-Path -LiteralPath $missingDestination)) 'FailedPreflightCreatesNoPackage'
    $pin=Get-Content -LiteralPath (Join-Path $PSScriptRoot '../tools/nr/runtime-pin.json') -Raw|ConvertFrom-Json
    $fifty=@($pin.profiles|Where-Object id -eq 'rtx50')[0]
    Check ($fifty.relativePath -eq 'NR/rtx40/nvngx_dlssnr.dll' -and
        $fifty.sha256 -eq 'e67dee209320cdafe0e93e45675d7aa34323a53acc57a72b2e40a181581c989a' -and
        $fifty.compatibility -eq 'CallerIdentityProbeRequired') 'RealRtx50PackageUsesPatchedSharedRuntime'
    $physical=@(Get-NrPhysicalModels $pin.profiles)
    Check ($physical.Count -eq 2 -and ($physical|Measure-Object bytes -Sum).Sum -eq 475512032) 'RealPackageSavesSeparateRtx50Payload'
} finally {
    # This random directory is created solely by this test and remains under TEMP.
    $full=[IO.Path]::GetFullPath($root)
    if(-not $full.StartsWith([IO.Path]::GetFullPath([IO.Path]::GetTempPath()),[StringComparison]::OrdinalIgnoreCase)){throw 'Unsafe test cleanup'}
    Remove-Item -LiteralPath $full -Recurse -Force
}
