[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$BuildDirectory,
    [Parameter(Mandatory)][string]$FsrRuntimeDirectory,
    [Parameter(Mandatory)][string]$SourceIni,
    [Parameter(Mandatory)][string]$OutputDirectory,
    [string]$NeuralRuntimeDirectory='',
    [switch]$FsrFrameGeneration
)
$ErrorActionPreference='Stop'
. (Join-Path $PSScriptRoot '../fsr/PackageCommon.ps1')
. (Join-Path $PSScriptRoot '../nr/RuntimePackageCommon.ps1')
$neuralTrial=-not [string]::IsNullOrWhiteSpace($NeuralRuntimeDirectory)
$repository=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$build=[IO.Path]::GetFullPath($BuildDirectory)
$cache=Get-Content -LiteralPath (Join-Path $build 'CMakeCache.txt') -Raw
if($cache -notmatch '(?m)^TRP_ENABLE_XESS:BOOL=ON\r?$'){throw 'XeSS compiled capability required'}
if($neuralTrial -and $cache -notmatch '(?m)^TRP_ENABLE_NEURAL_RENDERING:BOOL=ON\r?$'){throw 'NR compiled capability required'}
if($FsrFrameGeneration -and $cache -notmatch '(?m)^TRP_ENABLE_FSR_FG:BOOL=ON\r?$'){throw 'FSR FG compiled capability required'}
$dll=Join-Path $build 'Release/RaZkolbaS.dll'
$identity=Get-EmbeddedBuildIdentity $dll
$revision=$identity.sourceRevision
if(-not $identity.sourceClean){throw 'Trial requires a clean committed build identity'}
if(([Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($dll))) -notmatch '\[XeSS startup\]'){throw 'Compiled XeSS startup implementation absent'}
$kind=if($FsrFrameGeneration){'FSR FG'}elseif($neuralTrial){'NR'}else{'SR'}
$name="RaZKolbaS XeSS $kind trial - $revision"
$stage=Join-Path ([IO.Path]::GetFullPath($OutputDirectory)) $name
$archive=$stage+'.zip'
if((Test-Path -LiteralPath $stage) -or (Test-Path -LiteralPath $archive)){throw 'Immutable XeSS trial already exists'}
$sdk=Join-Path $repository 'out/research/xess-sdk-3.0.2'
$pin=Get-Content -LiteralPath (Join-Path $PSScriptRoot 'sdk-pin.json') -Raw | ConvertFrom-Json
$runtime=Join-Path $sdk 'SKSE/Plugins/RaZkolbaS/XeSS/libxess.dll'
Assert-PinnedFile $runtime $pin.runtime.sha256 $pin.runtime.bytes
$licensePin=@($pin.files | Where-Object path -eq 'LICENSE.txt')[0]
Assert-PinnedFile (Join-Path $sdk 'LICENSE.txt') $licensePin.sha256
$fsrPin=Get-Content -LiteralPath (Join-Path $PSScriptRoot '../fsr/runtime-pin.json') -Raw | ConvertFrom-Json
foreach($file in $fsrPin.runtime){Assert-PinnedFile (Join-Path $FsrRuntimeDirectory $file.filename) $file.sha256 $file.bytes}
$fgPin=Get-Content -LiteralPath (Join-Path $PSScriptRoot '../fsr/fg-runtime-pin.json') -Raw | ConvertFrom-Json
if($FsrFrameGeneration){foreach($file in $fgPin.runtime){Assert-PinnedFile (Join-Path $FsrRuntimeDirectory $file.filename) $file.sha256 $file.bytes}}
$plugins=Join-Path $stage 'SKSE/Plugins'
$resources=Join-Path $plugins 'RaZkolbaS'
[IO.Directory]::CreateDirectory((Join-Path $resources 'XeSS')) | Out-Null
[IO.Directory]::CreateDirectory((Join-Path $plugins 'FSR')) | Out-Null
Copy-Item -LiteralPath $dll -Destination (Join-Path $plugins 'RaZkolbaS.dll')
Copy-Item -LiteralPath $runtime -Destination (Join-Path $resources 'XeSS/libxess.dll')
Copy-Item -LiteralPath (Join-Path $sdk 'LICENSE.txt') -Destination (Join-Path $resources 'XeSS/NOTICE.txt')
Copy-Item -LiteralPath (Join-Path $repository 'LICENSE') -Destination (Join-Path $resources 'NOTICE.txt')
Copy-Item -LiteralPath (Join-Path $repository 'package/FSR-API-MIT-NOTICE.txt') -Destination (Join-Path $plugins 'FSR/NOTICE.txt')
Copy-Item -LiteralPath (Join-Path $repository 'package/SKSE/Plugins/RaZkolbaS/RCAS.hlsl') -Destination $resources
Copy-Item -LiteralPath (Join-Path $repository 'package/SKSE/Plugins/RaZkolbaSImGui.ini') -Destination $plugins
foreach($file in $fsrPin.runtime){Copy-Item -LiteralPath (Join-Path $FsrRuntimeDirectory $file.filename) -Destination (Join-Path $plugins 'FSR')}
if($FsrFrameGeneration){foreach($file in $fgPin.runtime){Copy-Item -LiteralPath (Join-Path $FsrRuntimeDirectory $file.filename) -Destination (Join-Path $plugins 'FSR')}}
if($neuralTrial){
    $nrPin=Get-Content -LiteralPath (Join-Path $PSScriptRoot '../nr/runtime-pin.json') -Raw | ConvertFrom-Json
    $sources=@{}
    foreach($model in @(Get-NrPhysicalModels $nrPin.profiles)){
        $sources[$model.relativePath]=Join-Path ([IO.Path]::GetFullPath($NeuralRuntimeDirectory)) $model.relativePath
    }
    Copy-NrRuntimeModels $nrPin.profiles $sources $resources
    Assert-NrRuntimeModels $nrPin.profiles $resources
}
$lines=Set-PackageIniValues ([IO.File]::ReadAllLines([IO.Path]::GetFullPath($SourceIni))) @{
    'Upscaling/Upscaler'='XeSS';'XeSS/Quality'='Native';'XeSS/SourceColorEncoding'='Gamma22';
    'Upscaling/EnableJitter'='true';'DLSS/Quality'='Native';'FSR/Quality'='Native';'FSR/Provider'='FSR3';
    'FSR/SourceColorEncoding'='Gamma22';'FrameGeneration/Backend'=$(if($FsrFrameGeneration){'FSR'}else{'Auto'});'FrameGeneration/Enabled'=([string][bool]$FsrFrameGeneration).ToLowerInvariant();
    'FrameGeneration/FsrProvider'='FSR3';'NeuralRendering/Enabled'=([string]$neuralTrial).ToLowerInvariant();'HDROutput/Enabled'='false';
    'DynamicResolution/Enabled'='false';'DynamicResolution/Oscillate'='false';
    'Runtime/NRDriverCore'='';'Runtime/NRRuntimeRoot'='';'Runtime/NRRuntimePath'='';
    'Debug/LogFrameDiagnostics'='false'
}
if($neuralTrial){
    $lines=Set-PackageIniValues $lines @{
        'NeuralRendering/Placement'=$(if($FsrFrameGeneration){'After'}else{'Before'});'NeuralRendering/PassCount'='1';
        'NeuralRendering Advanced/Profile'='Auto';'NeuralRendering Advanced/SourceColorEncoding'='Gamma22';
        'Debug/NRLegacyRuntime'='false';'NeuralRendering Advanced/SdrBytesTrial'='true'
    }
}
$lines=Set-PackageIniValues $lines @{'Interface/NativeUI'='true';'FrameGeneration/UIComposition'='Dedicated'}
[IO.File]::WriteAllLines((Join-Path $plugins 'RaZkolbaS.ini'),[string[]]$lines,[Text.UTF8Encoding]::new($false))
$metadata="[General]`nversion=1.3.5-xess-$($kind.ToLowerInvariant().Replace(' ','-'))-$revision`ninstallationFile=$name.zip`nnotes=Experimental XeSS Native; communityNR=$neuralTrial; FSR FG=$([bool]$FsrFrameGeneration); HDR off; FSR3 startup fallback; source=$revision`n"
[IO.File]::WriteAllText((Join-Path $stage 'meta.ini'),$metadata,[Text.UTF8Encoding]::new($false))
$imports=Get-PEImports (Join-Path $plugins 'RaZkolbaS.dll')
if($imports.module -match '^(libxess|libxess_fg|libxell)\.dll$'){throw 'Unexpected static XeSS runtime dependency'}
$settings=Read-PackageIni (Join-Path $plugins 'RaZkolbaS.ini') -Raw
$expectedBackend=if($FsrFrameGeneration){'2'}else{'0'}
if((Read-PackageIni (Join-Path $plugins 'RaZkolbaS.ini'))['FrameGeneration/Backend'] -ne $expectedBackend){throw 'XeSS trial presenter mismatch'}
foreach($item in @{'Upscaling/Upscaler'='XeSS';'XeSS/Quality'='Native';'XeSS/SourceColorEncoding'='Gamma22';'DLSS/Quality'='Native';'FrameGeneration/Enabled'=([string][bool]$FsrFrameGeneration).ToLowerInvariant();'NeuralRendering/Enabled'=([string]$neuralTrial).ToLowerInvariant();'HDROutput/Enabled'='false';'DynamicResolution/Enabled'='false'}.GetEnumerator()) {
    if($settings[$item.Key] -cne $item.Value){throw "Trial setting mismatch: $($item.Key)"}
}
if($neuralTrial){Assert-PortableNrPackageIni $settings}
Add-Type -AssemblyName System.IO.Compression.FileSystem
[IO.Compression.ZipFile]::CreateFromDirectory($stage,$archive,[IO.Compression.CompressionLevel]::Optimal,$false)
$receipt=[ordered]@{name=$name;stage=$stage;archive=$archive;sourceRevision=$revision;pluginSha256=(Get-FileHash -LiteralPath $dll -Algorithm SHA256).Hash.ToLowerInvariant();runtimeSha256=$pin.runtime.sha256;archiveSha256=(Get-FileHash -LiteralPath $archive -Algorithm SHA256).Hash.ToLowerInvariant();fsrFrameGeneration=[bool]$FsrFrameGeneration;gameplayQualified=$false}
$receipt | ConvertTo-Json | Set-Content -LiteralPath ($archive+'.receipt.json') -Encoding utf8
$receipt | ConvertTo-Json
