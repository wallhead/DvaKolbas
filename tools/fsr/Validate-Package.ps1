param([Parameter(Mandatory)][ValidateSet('Standard','Universal')][string]$Edition,[Parameter(Mandatory)][string]$PackageDirectory)
. (Join-Path $PSScriptRoot 'PackageCommon.ps1')
$root=[IO.Path]::GetFullPath($PackageDirectory)
$pin=Get-Content -LiteralPath (Join-Path $PSScriptRoot 'runtime-pin.json') -Raw | ConvertFrom-Json
$manifest=Get-Content -LiteralPath (Join-Path $root 'manifest.json') -Raw | ConvertFrom-Json
if($manifest.schema -ne 1 -or $manifest.edition -ne $Edition -or $manifest.sdkCommit -ne $pin.commit -or $manifest.sdkRelease -ne $pin.release){throw 'Package identity mismatch'}
$required=@('SKSE/Plugins/TheosRenderPipeline.dll','SKSE/Plugins/TheosRenderPipeline.ini','SKSE/Plugins/TheosRenderPipelineImGui.ini','SKSE/Plugins/TheosRenderPipeline/RCAS.hlsl','LICENSE','THIRD_PARTY_FSR.md','AMD-FidelityFX-license.md','FSR-API-MIT-NOTICE.txt','FSR_TEST_CHECKLIST.md')
$required+=@($pin.runtime | ForEach-Object {'SKSE/Plugins/FSR/'+$_.filename})
$seen=[Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
foreach($entry in $manifest.files) {
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
        if($relative -notin @('SKSE/Plugins/TheosRenderPipeline.dll','SKSE/Plugins/FSR/amd_fidelityfx_loader_dx12.dll','SKSE/Plugins/FSR/amd_fidelityfx_upscaler_dx12.dll')){throw "Unapproved DLL in FSR-only package: $relative"}
        Assert-NoVendorImports $file.FullName
    }
}
foreach($runtime in $pin.runtime){Assert-PinnedFile (Join-Path $root ('SKSE/Plugins/FSR/'+$runtime.filename)) $runtime.sha256 $runtime.bytes}
$license=$pin.headers | Where-Object {$_.path -eq 'Kits/FidelityFX/docs/license.md'}
Assert-PinnedFile (Join-Path $root 'AMD-FidelityFX-license.md') $license.sha256
$ini=Read-PackageIni (Join-Path $root 'SKSE/Plugins/TheosRenderPipeline.ini')
foreach($pair in @(@('Settings/UpscaleType','4'),@('FrameGeneration/Enabled','false'),@('Experimental/FrameGenerationBackend','0'),@('SourceDLSSG/NeuralRenderingEnabled','false'),@('HDROutput/Enabled','false'),@('DynamicResolution/Enabled','false'),@('DynamicResolution/Oscillate','false'))) {
    if($ini[$pair[0]] -ne $pair[1]){throw "Invalid FSR selector: $($pair[0])"}
}
if($ini['FSR/Quality'] -cnotin @('Quality','Balanced','Performance','NativeAA') -or $ini['FSR/ProviderPolicy'] -cnotin @('Analytical','Compatible')){throw 'Invalid FSR quality/provider'}
$sharpness=0.0
if(-not [double]::TryParse($ini['FSR/Sharpness'],[Globalization.NumberStyles]::Float,[Globalization.CultureInfo]::InvariantCulture,[ref]$sharpness) -or -not [double]::IsFinite($sharpness) -or $sharpness -lt 0 -or $sharpness -gt 1){throw 'Invalid FSR sharpness'}
if($ini.ContainsKey('Settings/EnableUpscaler') -and $ini['Settings/EnableUpscaler'] -ne 'true'){throw 'FSR upscaler disabled'}
foreach($key in @('Experimental/PureDarkFullDelegation','Experimental/EnableX3Presentation','Experimental/EnableXessCapabilityProbe','Experimental/EnableNeuralRenderingCapabilityProbe','Debug/AutoABTest')) {
    if($ini.ContainsKey($key) -and $ini[$key] -notin @('false','0','no','off')){throw "Unavailable renderer experiment: $key"}
}
foreach($key in @('Experimental/X3PresentationMode','Experimental/NeuralRenderingStartupMode')) {
    if($ini.ContainsKey($key) -and $ini[$key] -ne '0'){throw "Unavailable renderer experiment: $key"}
}
Write-Output "PASS: $Edition FSR-only package; exact pin/manifest/config/license and normal/delayed imports: $root"
