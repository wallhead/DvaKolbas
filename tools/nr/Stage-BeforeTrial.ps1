param(
    [Parameter(Mandatory)][string]$BuildDirectory,
    [Parameter(Mandatory)][string]$AcceptedModDirectory,
    [Parameter(Mandatory)][string]$DriverCore,
    [Parameter(Mandatory)][string]$Rtx40,
    [Parameter(Mandatory)][string]$Rtx20_30,
    [Parameter(Mandatory)][string]$OutputDirectory
)
# Stage only. Profile activation and launching Skyrim are separate operations.
. (Join-Path $PSScriptRoot 'RuntimePackageCommon.ps1')
$repository=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($OutputDirectory)
if(Test-Path -LiteralPath $root){throw "Trial output already exists: $root"}
$pin=Get-Content -LiteralPath (Join-Path $PSScriptRoot 'runtime-pin.json') -Raw | ConvertFrom-Json
$dll=Join-Path ([IO.Path]::GetFullPath($BuildDirectory)) 'Release/RaZkolbaS.dll'
$identity=Get-EmbeddedBuildIdentity $dll
if(-not $identity.sourceClean -or -not $identity.neuralRenderingCompiled -or -not $identity.fsrCompiled -or -not $identity.frameGenerationCompiled){throw 'Trial requires clean NR/SR/FG compiled source'}
Assert-NoVendorImports $dll
Assert-PinnedFile $DriverCore $pin.qualifiedProbeDriverCore.sha256 $pin.qualifiedProbeDriverCore.bytes
$models=@{'NR/rtx40/nvngx_dlssnr.dll'=$Rtx40;'NR/rtx20-30/nvngx_dlssnr.dll'=$Rtx20_30}
foreach($p in @(Get-NrPhysicalModels $pin.profiles)){Assert-PinnedFile $models[$p.relativePath] $p.sha256 $p.bytes}
$accepted=[IO.Path]::GetFullPath($AcceptedModDirectory)
$iniSource=Join-Path $accepted 'SKSE/Plugins/RaZkolbaS.ini'
$ini=Read-PackageIni $iniSource
foreach($pair in @(@('Settings/UpscaleType','4'),@('FSR/Quality','NativeAA'),@('FSR/ProviderPolicy','Analytical'),@('FSR/SourceColorEncoding','Gamma22'),@('HDROutput/Enabled','false'),@('DynamicResolution/Enabled','false'),@('FrameGeneration/Backend','2'),@('Settings/NativeUI','true'),@('Experimental/NativeUICompositionMode','0'))){
    if($ini[$pair[0]] -ne $pair[1]){throw "Reference trial setting differs: $($pair[0])"}
}
[IO.Directory]::CreateDirectory((Join-Path $root 'SKSE/Plugins/RaZkolbaS'))|Out-Null
Copy-Item -LiteralPath $dll -Destination (Join-Path $root 'SKSE/Plugins/RaZkolbaS.dll')
foreach($relative in @('SKSE/Plugins/RaZkolbaSImGui.ini','SKSE/Plugins/RaZkolbaS/RCAS.hlsl')){
    Copy-Item -LiteralPath (Join-Path $repository ('package/'+$relative)) -Destination (Join-Path $root $relative)
}
$amdPins=@((Get-Content -LiteralPath (Join-Path $PSScriptRoot '../fsr/runtime-pin.json') -Raw|ConvertFrom-Json).runtime)
$amdPins+=@((Get-Content -LiteralPath (Join-Path $PSScriptRoot '../fsr/fg-runtime-pin.json') -Raw|ConvertFrom-Json).runtime)
[IO.Directory]::CreateDirectory((Join-Path $root 'SKSE/Plugins/FSR'))|Out-Null
foreach($p in $amdPins){$file=Join-Path $accepted ('SKSE/Plugins/FSR/'+$p.filename);Assert-PinnedFile $file $p.sha256 $p.bytes;Copy-Item -LiteralPath $file -Destination (Join-Path $root 'SKSE/Plugins/FSR')}
Copy-NrRuntimeModels $pin.profiles $models (Join-Path $root 'SKSE/Plugins/RaZkolbaS')
$lines=[Collections.Generic.List[string]]::new();$lines.AddRange([string[]][IO.File]::ReadAllLines($iniSource));$section=''
$changes=@{
    'SourceDLSSG/NeuralRenderingEnabled'='true';'SourceDLSSG/NRBeforeUpscaling'='true';'SourceDLSSG/NRPasses'='1';
    'SourceDLSSG/NRPreset'='0';'SourceDLSSG/NRInputScale'='1';'SourceDLSSG/NRResolveMethod'='0';
    'SourceDLSSG/NRColorIsHDR'='false';'SourceDLSSG/NRPeripheralCompression'='false';'SourceDLSSG/NRFusedPreparation'='false';
    'SourceDLSSG/NRUICorrection'='false';'FrameGeneration/Enabled'='true';'Hotkeys/EnableNRHotkeys'='true';'Appearance/Enabled'='false'
}
if($ini['NeuralRendering/CommunityRuntime'] -eq 'true'){throw 'Reference INI already selects the community trial'}
$changes['NeuralRendering/CommunityRuntime']='true'
$changes['NeuralRendering/Profile']='Auto'
$changes['Runtime/NRRuntimeRoot']=''
$changes['Runtime/NRDriverCore']=''
$changes['NeuralRendering/SourceColorEncoding']='Gamma22'
$lines=ConvertTo-PortableNrPackageIni (Set-PackageIniValues $lines.ToArray() $changes)
[IO.File]::WriteAllLines((Join-Path $root 'SKSE/Plugins/RaZkolbaS.ini'),$lines,[Text.UTF8Encoding]::new($false))
Write-PortableModMetadata -Directory $root -Revision $identity.sourceRevision
Copy-Item -LiteralPath (Join-Path $repository 'docs/NR_BEFORE_TRIAL.md') -Destination (Join-Path $root 'README.md')
foreach($relative in @('LICENSE','THIRD_PARTY_FSR.md','AMD-FidelityFX-license.md','FSR-API-MIT-NOTICE.txt')){
    $source=if($relative -eq 'LICENSE'){Join-Path $repository 'LICENSE'}else{Join-Path $accepted $relative}
    Copy-Item -LiteralPath $source -Destination (Join-Path $root $relative)
}
$files=@(Get-ChildItem -LiteralPath $root -Recurse -File | Sort-Object FullName | ForEach-Object {[ordered]@{path=[IO.Path]::GetRelativePath($root,$_.FullName).Replace('\','/');bytes=$_.Length;sha256=(Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant()}})
[ordered]@{schema=1;scope='Native SDR Before only; Skyrim acceptance pending';buildIdentity=$identity;profiles=$pin.profiles;driverCore=$pin.qualifiedProbeDriverCore;corePath=[IO.Path]::GetFullPath($DriverCore);amdNr='Unsupported';files=$files}|ConvertTo-Json -Depth 12|Set-Content -LiteralPath (Join-Path $root 'nr-trial-manifest.json') -Encoding utf8
& (Join-Path $PSScriptRoot 'Validate-BeforeTrial.ps1') -PackageDirectory $root
Write-Output "STAGED NR trial: $root"
