param([Parameter(Mandatory)][string]$PackageDirectory)
. (Join-Path $PSScriptRoot 'RuntimePackageCommon.ps1')
$root=[IO.Path]::GetFullPath($PackageDirectory)
$manifest=Get-Content -LiteralPath (Join-Path $root 'nr-trial-manifest.json') -Raw|ConvertFrom-Json
$pin=Get-Content -LiteralPath (Join-Path $PSScriptRoot 'runtime-pin.json') -Raw|ConvertFrom-Json
$identity=Get-EmbeddedBuildIdentity (Join-Path $root 'SKSE/Plugins/RaZkolbaS.dll')
if(-not $identity.sourceClean -or -not $identity.neuralRenderingCompiled -or -not $identity.fsrCompiled -or -not $identity.frameGenerationCompiled){throw 'Missing clean compiled capability'}
if(($identity|ConvertTo-Json -Compress) -ne ($manifest.buildIdentity|ConvertTo-Json -Compress)){throw 'Compiled source differs from manifest'}
Assert-NoVendorImports (Join-Path $root 'SKSE/Plugins/RaZkolbaS.dll')
$seen=[Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
foreach($f in $manifest.files){
    if([IO.Path]::IsPathRooted($f.path) -or $f.path -match '(^|[\\/])\.\.([\\/]|$)|:'){throw 'Unsafe manifest path'}
    $file=[IO.Path]::GetFullPath((Join-Path $root $f.path))
    if(-not $file.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase) -or -not $seen.Add($f.path)){throw 'Duplicate/escaping manifest path'}
    Assert-PinnedFile $file $f.sha256 $f.bytes
}
foreach($file in Get-ChildItem -LiteralPath $root -Recurse -File){
    $relative=[IO.Path]::GetRelativePath($root,$file.FullName).Replace('\','/')
    if($relative -ne 'nr-trial-manifest.json' -and -not $seen.Contains($relative)){throw "Unrecorded trial file: $relative"}
}
if(($manifest.profiles|ConvertTo-Json -Depth 8 -Compress) -ne ($pin.profiles|ConvertTo-Json -Depth 8 -Compress)){throw 'Manifest NR profile catalog differs from current pins'}
Assert-NrRuntimeModels $pin.profiles (Join-Path $root 'SKSE/Plugins/RaZkolbaS')
Assert-PinnedFile $manifest.corePath $pin.qualifiedProbeDriverCore.sha256 $pin.qualifiedProbeDriverCore.bytes
$amd=@((Get-Content -LiteralPath (Join-Path $PSScriptRoot '../fsr/runtime-pin.json') -Raw|ConvertFrom-Json).runtime)
$amd+=@((Get-Content -LiteralPath (Join-Path $PSScriptRoot '../fsr/fg-runtime-pin.json') -Raw|ConvertFrom-Json).runtime)
foreach($p in $amd){Assert-PinnedFile (Join-Path $root ('SKSE/Plugins/FSR/'+$p.filename)) $p.sha256 $p.bytes}
$ini=Read-PortableNrPackageIni (Join-Path $root 'SKSE/Plugins/RaZkolbaS.ini')
foreach($pair in @(@('NeuralRendering/CommunityRuntime','true'),@('NeuralRendering/Profile','Auto'),@('NeuralRendering/SourceColorEncoding','Gamma22'),@('SourceDLSSG/NeuralRenderingEnabled','true'),@('SourceDLSSG/NRBeforeUpscaling','true'),@('SourceDLSSG/NRPasses','1'),@('SourceDLSSG/NRInputScale','1'),@('SourceDLSSG/NRPreset','0'),@('SourceDLSSG/NRResolveMethod','0'),@('SourceDLSSG/NRUICorrection','false'),@('SourceDLSSG/NRColorIsHDR','false'),@('SourceDLSSG/NRPeripheralCompression','false'),@('SourceDLSSG/NRFusedPreparation','false'),@('FrameGeneration/Enabled','true'),@('Settings/UpscaleType','4'),@('FrameGeneration/Backend','2'),@('Experimental/NativeUICompositionMode','0'),@('Settings/NativeUI','true'),@('FSR/Quality','NativeAA'),@('FSR/ProviderPolicy','Analytical'),@('FSR/SourceColorEncoding','Gamma22'),@('HDROutput/Enabled','false'),@('DynamicResolution/Enabled','false'),@('Appearance/Enabled','false'))){if($ini[$pair[0]] -ne $pair[1]){throw "Invalid trial selector: $($pair[0])"}}
Write-Output 'PASS: clean NR Before trial, three logical profiles / two physical models and AMD/runtime/core pins, full manifest and SDR/native/world settings'
