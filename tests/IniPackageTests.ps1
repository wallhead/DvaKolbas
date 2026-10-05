param([Parameter(Mandatory)][string]$Repository,[Parameter(Mandatory)][string]$Output)
$ErrorActionPreference='Stop'
. (Join-Path $Repository 'tools/fsr/PackageCommon.ps1')
[IO.Directory]::CreateDirectory($Output)|Out-Null
$source=Join-Path $Output 'legacy.ini'
$converted=Join-Path $Output ('converted-'+[Guid]::NewGuid().ToString('N')+'.ini')
[IO.File]::WriteAllText($source,@'
[Settings]
UpscaleType=3
QualityLevel=4
DLSSPreset=11
[FrameGeneration]
Enabled=false
[Experimental]
FrameGenerationBackend=1
SourceDLSSGStreamlineDirectory=Relative/SL
NeuralRenderingRuntimePath=Absolute/NR.dll
; User note must remain beside the unknown setting.
Keep=unchanged
[SourceDLSSG]
NeuralRenderingEnabled=true
NRBeforeUpscaling=false
NRStableColors=true
NRLocalTone=0
NRPasses=1
NRPass2UseSameSettings=false
NRPass2Style=2
NRPass3UseSameSettings=false
NRPass3Style=4
[NeuralRendering]
DriverCore=C:/driver/_nvngx.dll
RuntimeRoot=
'@,[Text.UTF8Encoding]::new($false))
$hash=(Get-FileHash -LiteralPath $source).Hash
& (Join-Path $Repository 'tools/ini/Reorganize-Ini.ps1') -SourceIni $source -OutputIni $converted
$raw=Read-PackageIni $converted -Raw
if($raw.ContainsKey('NeuralRendering/StableColors') -or $raw.ContainsKey('SourceDLSSG/NRStableColors')){
    throw 'Retired stable-color setting must not survive conversion'
}
if($raw['NR PASS 3/Style'] -ne '4' -or $raw['NR PASS 3/UseSameSettings'] -ne 'false'){
    throw 'Independent third-pass settings must survive conversion'
}
if($raw['NR PASS 1/Tone'] -ne '0' -or $raw['NR PASS 2/UseSameSettings'] -ne 'false' -or
    $raw['FrameGeneration/Enabled'] -ne 'false' -or $raw['Runtime/NRRuntimeRoot'] -ne '' -or
    $raw.ContainsKey('NR PASS 2/Tone') -or $raw.ContainsKey('SourceDLSSG/NRLocalTone')){
    throw 'Migration changed explicit zero/false/empty or filled missing pass inheritance'
}
if((Get-FileHash -LiteralPath $source).Hash -ne $hash){throw 'Converter mutated source'}
if([IO.File]::ReadAllText($converted) -notmatch 'User note must remain'){throw 'Unknown key comment lost'}
$changes=@{'SourceDLSSG/NRBeforeUpscaling'='true';'NeuralRendering/CommunityRuntime'='true';'NeuralRendering/DriverCore'='New/core.dll'}
$lines=Set-PackageIniValues ([IO.File]::ReadAllLines($converted)) $changes
$updated=Join-Path $Output 'updated.ini';[IO.File]::WriteAllLines($updated,$lines)
$view=Read-PackageIni $updated
if($view['SourceDLSSG/NRBeforeUpscaling'] -ne 'true' -or $view['NeuralRendering/CommunityRuntime'] -ne 'true' -or
    $view['NeuralRendering/DriverCore'] -ne 'New/core.dll' -or $view['SourceDLSSG/NRLocalTone'] -ne '0'){
    throw 'New-layout packaging selectors do not match runtime readers'
}
# Valid mixed files must be accepted and both aliases updated consistently.
$mixed=@('[SourceDLSSG]','NRBeforeUpscaling=true','[NeuralRendering]','BeforeUpscaling=false')
$mixedPath=Join-Path $Output 'mixed.ini'
[IO.File]::WriteAllLines($mixedPath,(Set-PackageIniValues $mixed @{'SourceDLSSG/NRBeforeUpscaling'='false'}))
$mixedRaw=Read-PackageIni $mixedPath -Raw
if($mixedRaw['SourceDLSSG/NRBeforeUpscaling'] -ne 'false' -or $mixedRaw['NeuralRendering/BeforeUpscaling'] -ne 'false'){
    throw 'Mixed-layout placement update must accept and synchronize both aliases'
}
$rejected=$false
try {& (Join-Path $Repository 'tools/ini/Reorganize-Ini.ps1') -SourceIni $source -OutputIni $source}catch{$rejected=$true}
if(-not $rejected -or (Get-FileHash -LiteralPath $source).Hash -ne $hash){throw 'Source overwrite guard failed'}
Write-Output 'PASS: conversion preserves values, absent defaults, unknown comments and packaging selectors'
