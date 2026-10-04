param(
    [Parameter(Mandatory)][string]$BuildDirectory,
    [Parameter(Mandatory)][string]$AcceptedModDirectory,
    [Parameter(Mandatory)][string]$OutputDirectory
)
$ErrorActionPreference='Stop'
. (Join-Path $PSScriptRoot '../fsr/PackageCommon.ps1')
$repository=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$accepted=(Resolve-Path -LiteralPath $AcceptedModDirectory).Path
$output=[IO.Path]::GetFullPath($OutputDirectory)
$allowed=[IO.Path]::GetFullPath((Join-Path $repository 'out/packages'))+[IO.Path]::DirectorySeparatorChar
if(-not $output.StartsWith($allowed,[StringComparison]::OrdinalIgnoreCase) -or (Test-Path -LiteralPath $output)){
    throw 'Use a new stage directory inside this worktree/out/packages'
}
$dll=Join-Path (Resolve-Path -LiteralPath $BuildDirectory).Path 'Release/TheosRenderPipeline.dll'
$identity=Get-EmbeddedBuildIdentity $dll
if(-not $identity.sourceClean -or -not $identity.neuralRenderingCompiled -or -not $identity.fsrCompiled -or -not $identity.frameGenerationCompiled){
    throw 'Post-SR trial needs a clean source NR/SR/FG build'
}
Assert-NoVendorImports $dll
$iniRelative='SKSE/Plugins/TheosRenderPipeline.ini'
$iniPath=Join-Path $accepted $iniRelative
$ini=Read-PackageIni $iniPath
foreach($pair in @(@('NeuralRendering/CommunityRuntime','true'),@('NeuralRendering/SdrBytesTrial','true'),@('SourceDLSSG/NRStableColors','false'),@('SourceDLSSG/NRPasses','1'),@('SourceDLSSG/NRPreset','0'),@('SourceDLSSG/NRColorIsHDR','false'),@('SourceDLSSG/NRResolveMethod','0'),@('SourceDLSSG/NRPeripheralCompression','false'),@('SourceDLSSG/NRFusedPreparation','false'),@('SourceDLSSG/NRUICorrection','false'),@('Settings/NativeUI','true'),@('DynamicResolution/Enabled','false'),@('HDROutput/Enabled','false'))){
    if($ini[$pair[0]] -ne $pair[1]){throw "Unqualified reference setting: $($pair[0])"}
}
if([double]::Parse($ini['SourceDLSSG/NRInputScale'],[Globalization.CultureInfo]::InvariantCulture) -ne 1){throw 'Native NR input scale required'}
if($ini['NeuralRendering/SourceColorEncoding'] -notin @('Gamma22','SRGB')){throw 'Encoded SDR NR source required'}
$dlaa=$ini['Settings/UpscaleType'] -eq '3' -and $ini['Experimental/FrameGenerationBackend'] -eq '1'
$fsr=$ini['Settings/UpscaleType'] -eq '4' -and $ini['FSR/Quality'] -eq 'NativeAA' -and $ini['FSR/SourceColorEncoding'] -eq $ini['NeuralRendering/SourceColorEncoding']
if(-not ($dlaa -or $fsr)){throw 'After trial requires the accepted DLAA or FSR Native AA route'}
$pin=Get-Content -LiteralPath (Join-Path $PSScriptRoot 'runtime-pin.json') -Raw | ConvertFrom-Json
Assert-PinnedFile $ini['NeuralRendering/DriverCore'] $pin.qualifiedProbeDriverCore.sha256 $pin.qualifiedProbeDriverCore.bytes
if($ini['NeuralRendering/RuntimeRoot']){throw 'Stage currently requires packaged, relative NR model paths'}
foreach($profile in $pin.profiles){Assert-PinnedFile (Join-Path $accepted ('SKSE/Plugins/TheosRenderPipeline/'+$profile.relativePath)) $profile.sha256 $profile.bytes}
$protected=@{}
foreach($file in Get-ChildItem -LiteralPath $accepted -Recurse -File){
    $relative=[IO.Path]::GetRelativePath($accepted,$file.FullName).Replace('\','/')
    $protected[$relative]=(Get-FileHash -LiteralPath $file.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
}
[IO.Directory]::CreateDirectory($output)|Out-Null
foreach($item in Get-ChildItem -LiteralPath $accepted){Copy-Item -LiteralPath $item.FullName -Destination $output -Recurse}
Copy-Item -LiteralPath $dll -Destination (Join-Path $output 'SKSE/Plugins/TheosRenderPipeline.dll')
$lines=[IO.File]::ReadAllLines($iniPath);$section='';$changed=0
for($i=0;$i -lt $lines.Length;$i++){
    if($lines[$i].Trim() -match '^\[([^\]]+)\]$'){$section=$Matches[1];continue}
    if($section -eq 'SourceDLSSG' -and $lines[$i] -match '^\s*NRBeforeUpscaling\s*='){$lines[$i]='NRBeforeUpscaling = false';$changed++}
}
if($changed -ne 1){throw 'Reference needs exactly one explicit placement key'}
[IO.File]::WriteAllLines((Join-Path $output $iniRelative),$lines,[Text.UTF8Encoding]::new($false))
$stagedIni=Read-PackageIni (Join-Path $output $iniRelative)
foreach($key in $ini.Keys){if($key -ne 'SourceDLSSG/NRBeforeUpscaling' -and $stagedIni[$key] -ne $ini[$key]){throw "Unexpected INI change: $key"}}
Copy-Item -LiteralPath (Join-Path $repository 'docs/NR_POST_SR_TRIAL.md') -Destination (Join-Path $output 'POST_SR_TRIAL.md')
$manifest=@()
foreach($file in Get-ChildItem -LiteralPath $output -Recurse -File){
    $relative=[IO.Path]::GetRelativePath($output,$file.FullName).Replace('\','/')
    $hash=(Get-FileHash -LiteralPath $file.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
    $manifest+=@{path=$relative;bytes=$file.Length;sha256=$hash}
    if($protected.ContainsKey($relative) -and $relative -notin @($iniRelative,'SKSE/Plugins/TheosRenderPipeline.dll') -and $hash -ne $protected[$relative]){throw "Unexpected staged change: $relative"}
}
foreach($relative in $protected.Keys){if((Get-FileHash -LiteralPath (Join-Path $accepted $relative) -Algorithm SHA256).Hash.ToLowerInvariant() -ne $protected[$relative]){throw "Reference changed during staging: $relative"}}
@{schema=1;scope='Local Native-AA post-SR trial; not installed or game-tested';identity=$identity;sourceMod=$accepted;iniChangedKeys=@('SourceDLSSG/NRBeforeUpscaling');sourceFiles=$protected;files=$manifest} |
    ConvertTo-Json -Depth 8 | Set-Content -LiteralPath (Join-Path $output 'post-sr-manifest.json') -Encoding utf8
Write-Output "STAGED ONLY: $output"
