param([Parameter(Mandatory)][string]$BuildDirectory,[Parameter(Mandatory)][string]$RuntimeDirectory,[Parameter(Mandatory)][string]$ScratchRoot,[ValidateSet('Standard','Universal')][string]$Edition='Standard')
$ErrorActionPreference='Stop'
. (Join-Path $PSScriptRoot '../tools/fsr/PackageCommon.ps1')
$root=Join-Path ([IO.Path]::GetFullPath($ScratchRoot)) ([guid]::NewGuid().ToString('N'))
$stage=Join-Path $PSScriptRoot '../tools/fsr/Stage-Package.ps1'
$validate=Join-Path $PSScriptRoot '../tools/fsr/Validate-Package.ps1'
function Rejected([scriptblock]$Action,[string]$Case){$failed=$false;try{& $Action}catch{$failed=$true};if(-not $failed){throw "$Case accepted invalid package"};Write-Output "PASS: $Case rejected"}
& $stage -Edition $Edition -BuildDirectory $BuildDirectory -RuntimeDirectory $RuntimeDirectory -OutputDirectory $root
$package=Join-Path $root ($Edition+'-FSR-SR')
& $validate -Edition $Edition -PackageDirectory $package
if(Get-ChildItem -LiteralPath $package -Recurse -File | Where-Object {$_.Name -match '^(sl\.|_?nvngx|nvapi)'} ){throw 'NvidiaFreePackage failed'}
$manifest=Get-Content -LiteralPath (Join-Path $package 'manifest.json') -Raw | ConvertFrom-Json
$originalManifest=$manifest | ConvertTo-Json -Depth 10
foreach($entry in $manifest.files){if((Get-FileHash -LiteralPath (Join-Path $package $entry.path) -Algorithm SHA256).Hash.ToLowerInvariant() -ne $entry.sha256){throw 'RuntimeManifestMatchesFiles failed'}}
$runtime=Join-Path $package 'SKSE/Plugins/FSR/amd_fidelityfx_loader_dx12.dll'
[IO.File]::AppendAllText($runtime,'modified runtime')
Rejected {& $validate -Edition $Edition -PackageDirectory $package} 'WrongRuntimePackageRejected'
# Updating a self-authored manifest cannot bless a runtime differing from the pin.
foreach($entry in $manifest.files){if($entry.path -like '*amd_fidelityfx_loader_dx12.dll'){$entry.sha256=(Get-FileHash -LiteralPath $runtime -Algorithm SHA256).Hash.ToLowerInvariant();$entry.bytes=(Get-Item -LiteralPath $runtime).Length}}
$manifest | ConvertTo-Json -Depth 10 | Set-Content -LiteralPath (Join-Path $package 'manifest.json') -Encoding utf8
Rejected {& $validate -Edition $Edition -PackageDirectory $package} 'WrongRuntimeWithUpdatedManifest'
Copy-Item -LiteralPath (Join-Path $RuntimeDirectory 'amd_fidelityfx_loader_dx12.dll') -Destination $runtime
$originalManifest | Set-Content -LiteralPath (Join-Path $package 'manifest.json') -Encoding utf8
$shader=Join-Path $package 'SKSE/Plugins/TheosRenderPipeline/RCAS.hlsl'
$shaderBytes=[IO.File]::ReadAllBytes($shader)
Remove-Item -LiteralPath $shader
Rejected {& $validate -Edition $Edition -PackageDirectory $package} 'IncompletePackage'
[IO.File]::WriteAllBytes($shader,$shaderBytes)
$otherEdition=if($Edition -eq 'Standard'){'Universal'}else{'Standard'}
Rejected {& $validate -Edition $otherEdition -PackageDirectory $package} 'WrongEdition'
$manifest=$originalManifest | ConvertFrom-Json
$extra=Join-Path $package 'SKSE/Plugins/FSR/amd_fidelityfx_framegeneration_dx12.dll'
Copy-Item -LiteralPath $runtime -Destination $extra
$manifest.files+=@{path='SKSE/Plugins/FSR/amd_fidelityfx_framegeneration_dx12.dll';bytes=(Get-Item -LiteralPath $extra).Length;sha256=(Get-FileHash -LiteralPath $extra -Algorithm SHA256).Hash.ToLowerInvariant()}
$manifest | ConvertTo-Json -Depth 10 | Set-Content -LiteralPath (Join-Path $package 'manifest.json') -Encoding utf8
Rejected {& $validate -Edition $Edition -PackageDirectory $package} 'ExtraAmdRuntime'
Remove-Item -LiteralPath $extra
$originalManifest | Set-Content -LiteralPath (Join-Path $package 'manifest.json') -Encoding utf8
$config=Join-Path $package 'SKSE/Plugins/TheosRenderPipeline.ini'
$configOriginal=[IO.File]::ReadAllText($config)
foreach($encoding in @('Unknown','Guess','')) {
    $lines=Set-PackageIniValues ($configOriginal -split '\r?\n') @{'FSR/SourceColorEncoding'=$encoding}
    [IO.File]::WriteAllLines($config,$lines)
    if((Read-PackageIni $config)['FSR/SourceColorEncoding'] -cne $encoding){throw 'Color mutation did not apply'}
    $manifest=$originalManifest | ConvertFrom-Json
    foreach($entry in $manifest.files){if($entry.path -eq 'SKSE/Plugins/TheosRenderPipeline.ini'){$entry.bytes=(Get-Item -LiteralPath $config).Length;$entry.sha256=(Get-FileHash -LiteralPath $config -Algorithm SHA256).Hash.ToLowerInvariant()}}
    $manifest | ConvertTo-Json -Depth 10 | Set-Content -LiteralPath (Join-Path $package 'manifest.json') -Encoding utf8
    Rejected {& $validate -Edition $Edition -PackageDirectory $package} "UnspecifiedOrInvalidColor-$encoding"
}
[IO.File]::WriteAllLines($config,(Set-PackageIniValues ($configOriginal -split '\r?\n') @{'FSR/Quality'='quality'}))
if((Read-PackageIni $config)['FSR/Quality'] -cne 'quality'){throw 'Quality mutation did not apply'}
$manifest=$originalManifest | ConvertFrom-Json
foreach($entry in $manifest.files){if($entry.path -eq 'SKSE/Plugins/TheosRenderPipeline.ini'){$entry.bytes=(Get-Item -LiteralPath $config).Length;$entry.sha256=(Get-FileHash -LiteralPath $config -Algorithm SHA256).Hash.ToLowerInvariant()}}
$manifest | ConvertTo-Json -Depth 10 | Set-Content -LiteralPath (Join-Path $package 'manifest.json') -Encoding utf8
Rejected {& $validate -Edition $Edition -PackageDirectory $package} 'InvalidCaseQuality'
[IO.File]::WriteAllLines($config,(Set-PackageIniValues ($configOriginal -split '\r?\n') @{'Experimental/PureDarkFullDelegation'='true'}))
if((Read-PackageIni $config)['Experimental/PureDarkFullDelegation'] -ne 'true'){throw 'Delegation mutation did not apply'}
$manifest=$originalManifest | ConvertFrom-Json
foreach($entry in $manifest.files){if($entry.path -eq 'SKSE/Plugins/TheosRenderPipeline.ini'){$entry.bytes=(Get-Item -LiteralPath $config).Length;$entry.sha256=(Get-FileHash -LiteralPath $config -Algorithm SHA256).Hash.ToLowerInvariant()}}
$manifest | ConvertTo-Json -Depth 10 | Set-Content -LiteralPath (Join-Path $package 'manifest.json') -Encoding utf8
Rejected {& $validate -Edition $Edition -PackageDirectory $package} 'UnsupportedDelegation'
Write-Output 'PASS: NvidiaFreePackage RuntimeManifestMatchesFiles WrongRuntimePackageRejected incomplete and edition checks'
