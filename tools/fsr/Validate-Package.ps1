param([Parameter(Mandatory)][ValidateSet('Standard','Universal')][string]$Edition,[Parameter(Mandatory)][string]$PackageDirectory,[switch]$FrameGeneration,[switch]$RequireCleanSource)
. (Join-Path $PSScriptRoot 'PackageCommon.ps1')
$root=[IO.Path]::GetFullPath($PackageDirectory)
$pin=Get-Content -LiteralPath (Join-Path $PSScriptRoot 'runtime-pin.json') -Raw | ConvertFrom-Json
$manifest=Get-Content -LiteralPath (Join-Path $root 'manifest.json') -Raw | ConvertFrom-Json
$runtimePins=@($pin.runtime)
if($FrameGeneration){$fgPin=Get-Content -LiteralPath (Join-Path $PSScriptRoot 'fg-runtime-pin.json') -Raw|ConvertFrom-Json;$runtimePins+=@($fgPin.runtime)}
if($manifest.frameGenerationImplemented -ne [bool]$FrameGeneration){throw 'Package FG selection mismatch'}
$identity=Get-EmbeddedBuildIdentity (Join-Path $root 'SKSE/Plugins/RaZkolbaS.dll')
if($identity.edition -ne $Edition -or -not $identity.fsrCompiled -or ($FrameGeneration -and -not $identity.frameGenerationCompiled)){throw 'Compiled package capability mismatch'}
if(($identity|ConvertTo-Json -Compress) -ne ($manifest.buildIdentity|ConvertTo-Json -Compress)){throw 'Embedded source/capability marker differs from manifest'}
if($manifest.fsrEnabled -ne $identity.fsrCompiled -or $manifest.frameGenerationCompiled -ne $identity.frameGenerationCompiled -or $manifest.neuralRenderingCompiled -ne $identity.neuralRenderingCompiled){throw 'Manifest capability differs from compiled DLL'}
if([bool]$manifest.xessFrameGenerationCompiled -ne $identity.xessFrameGenerationCompiled){throw 'Manifest Intel FG capability differs from compiled DLL'}
$expectedProviders=@()
if($FrameGeneration){$expectedProviders=@($fgPin.observedProviders)}
if((ConvertTo-Json -InputObject @($manifest.providerVersions) -Compress) -ne (ConvertTo-Json -InputObject $expectedProviders -Compress)){throw 'Recorded provider versions differ from pinned observations'}
if($RequireCleanSource -and -not $identity.sourceClean){throw 'Final package requires an embedded clean source revision'}
if($manifest.schema -ne 2 -or $manifest.edition -ne $Edition -or $manifest.sdkCommit -ne $pin.commit -or $manifest.sdkRelease -ne $pin.release){throw 'Package identity mismatch'}
$required=@('SKSE/Plugins/RaZkolbaS.dll','SKSE/Plugins/RaZkolbaS.ini','SKSE/Plugins/RaZkolbaSImGui.ini','SKSE/Plugins/RaZkolbaS/RCAS.hlsl','LICENSE','THIRD_PARTY_FSR.md','AMD-FidelityFX-license.md','FSR-API-MIT-NOTICE.txt','FSR_TEST_CHECKLIST.md')
$required+=@($runtimePins | ForEach-Object {'SKSE/Plugins/FSR/'+$_.filename})
$seen=[Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
foreach($entry in $manifest.files) {
    if($entry.path.Replace('\','/') -match '^SKSE/Plugins/RaZkolbaS/Audio(/|$)'){throw 'Release packages must exclude the private audio easter egg'}
    if([IO.Path]::IsPathRooted($entry.path) -or $entry.path -match '(^|[\\/])\.\.([\\/]|$)|:'){throw 'Unsafe manifest path'}
    $absolute=[IO.Path]::GetFullPath((Join-Path $root $entry.path))
    if(-not $absolute.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase) -or -not $seen.Add($entry.path.Replace('\','/'))){throw 'Duplicate or escaping manifest path'}
    Assert-PinnedFile $absolute $entry.sha256 $entry.bytes
}
foreach($relative in $required){if(-not $seen.Contains($relative)){throw "Manifest omits required file: $relative"}}
foreach($file in Get-ChildItem -LiteralPath $root -Recurse -File) {
    $relative=[IO.Path]::GetRelativePath($root,$file.FullName).Replace('\','/')
    if($relative -eq 'manifest.json'){continue}
    if(-not $seen.Contains($relative)){throw "Unrecorded package file: $relative"}
    if($file.Name -match '^(sl\.|_?nvngx|nvapi)'){throw "NVIDIA runtime in FSR-only package: $relative"}
    if($file.Extension -ieq '.dll'){
        if($relative -notin (@('SKSE/Plugins/RaZkolbaS.dll')+@($runtimePins|ForEach-Object {'SKSE/Plugins/FSR/'+$_.filename}))){throw "Unapproved DLL in FSR-only package: $relative"}
        Assert-NoVendorImports $file.FullName
        $imports=@(Get-PEImports $file.FullName)
        if((ConvertTo-Json -InputObject $imports -Compress) -ne (ConvertTo-Json -InputObject @($manifest.dllImports.$relative) -Compress)){throw "Recorded normal/delayed imports differ: $relative"}
    }
}
foreach($runtime in $runtimePins){Assert-PinnedFile (Join-Path $root ('SKSE/Plugins/FSR/'+$runtime.filename)) $runtime.sha256 $runtime.bytes}
$license=$pin.headers | Where-Object {$_.path -eq 'Kits/FidelityFX/docs/license.md'}
Assert-PinnedFile (Join-Path $root 'AMD-FidelityFX-license.md') $license.sha256
$ini=Read-PackageIni (Join-Path $root 'SKSE/Plugins/RaZkolbaS.ini')
foreach($pair in @(@('Settings/UpscaleType','4'),@('FrameGeneration/Enabled',$(if($FrameGeneration){'true'}else{'false'})),@('FrameGeneration/Backend',$(if($FrameGeneration){'2'}else{'0'})),@('SourceDLSSG/NeuralRenderingEnabled','false'),@('HDROutput/Enabled','false'),@('DynamicResolution/Enabled','false'),@('DynamicResolution/Oscillate','false'))) {
    if($ini[$pair[0]] -ne $pair[1]){throw "Invalid FSR selector: $($pair[0])"}
}
if($FrameGeneration -and ($ini['Settings/NativeUI'] -ne 'true' -or $ini['Experimental/NativeUICompositionMode'] -ne '0')){throw 'FG requires dedicated native UI'}
if($ini['FSR/Quality'] -cnotin @('Quality','Balanced','Performance','NativeAA') -or $ini['FSR/ProviderPolicy'] -cnotin @('Analytical','Compatible','MachineLearning')){throw 'Invalid FSR quality/provider'}
if($ini['FSR/SourceColorEncoding'] -cnotin @('Linear','Gamma22','SRGB')){throw 'FSR package requires an explicit source color encoding'}
$sharpness=0.0
if(-not [double]::TryParse($ini['FSR/Sharpness'],[Globalization.NumberStyles]::Float,[Globalization.CultureInfo]::InvariantCulture,[ref]$sharpness) -or -not [double]::IsFinite($sharpness) -or $sharpness -lt 0 -or $sharpness -gt 1){throw 'Invalid FSR sharpness'}
if($ini.ContainsKey('Settings/EnableUpscaler') -and $ini['Settings/EnableUpscaler'] -ne 'true'){throw 'FSR upscaler disabled'}
foreach($key in @('Experimental/PureDarkFullDelegation','Experimental/EnableX3Presentation','Experimental/EnableXessCapabilityProbe','Experimental/EnableNeuralRenderingCapabilityProbe','Debug/AutoABTest')) {
    if($ini.ContainsKey($key) -and $ini[$key] -notin @('false','0','no','off')){throw "Unavailable renderer experiment: $key"}
}
foreach($key in @('Experimental/X3PresentationMode','Experimental/NeuralRenderingStartupMode')) {
    if($ini.ContainsKey($key) -and $ini[$key] -ne '0'){throw "Unavailable renderer experiment: $key"}
}
Write-Output "PASS: $Edition FSR package FG=$FrameGeneration; exact pin/manifest/config/license and normal/delayed imports: $root"
