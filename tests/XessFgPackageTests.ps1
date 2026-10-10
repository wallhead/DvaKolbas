param([Parameter(Mandatory)][string]$SdkDirectory,[Parameter(Mandatory)][string]$ScratchRoot)
$ErrorActionPreference='Stop'
. (Join-Path $PSScriptRoot '../tools/xess/FgPackageCommon.ps1')
function Require([bool]$Value,[string]$Reason){if(-not $Value){throw "FAIL: $Reason"}}
function Rejected([scriptblock]$Action,[string]$Reason){$failed=$false;try{& $Action}catch{$failed=$true};Require $failed $Reason}
$scratch=[IO.Path]::GetFullPath((Join-Path $ScratchRoot ([guid]::NewGuid().ToString('N'))))
[IO.Directory]::CreateDirectory($scratch)|Out-Null
$marker=Join-Path $scratch 'identity.bin'
[IO.File]::WriteAllText($marker,'TRP_BUILD|edition=Universal|source=123456789abc|FSR=1|FG=0|NR=1|XESS=1|XESSFG=1')
$identity=Get-EmbeddedBuildIdentity $marker
Require ($identity.xessFrameGenerationCompiled -and $identity.xessCompiled -and -not $identity.frameGenerationCompiled) 'Intel capability is independent of FSR FG'
Assert-XessGenerationBuild $identity
$identity.xessFrameGenerationCompiled=$false;Rejected {Assert-XessGenerationBuild $identity} 'unbuilt Intel capability rejected';$identity.xessFrameGenerationCompiled=$true
$identity.sourceClean=$false;Rejected {Assert-XessGenerationBuild $identity} 'modified source identity rejected';$identity.sourceClean=$true
$receiptPath=Join-Path $scratch 'receipt.json'
$receipt=@{schema=1;allChecksPassed=$true;sourceRevision=$identity.sourceRevision;pluginSha256=(Get-FileHash -LiteralPath $marker -Algorithm SHA256).Hash.ToLowerInvariant()}
$receipt|ConvertTo-Json|Set-Content -LiteralPath $receiptPath;Assert-XessValidationReceipt $receiptPath $marker
$receipt.pluginSha256='wrong';$receipt|ConvertTo-Json|Set-Content -LiteralPath $receiptPath
Rejected {Assert-XessValidationReceipt $receiptPath $marker} 'mismatched validated build receipt rejected'
$pin=Get-Content -LiteralPath (Join-Path $PSScriptRoot '../tools/xess/sdk-pin.json') -Raw | ConvertFrom-Json
foreach($file in $pin.generationRuntimes){
    Rejected {Assert-XessGenerationPayload $scratch $pin} 'incomplete paired FG/XeLL payload rejected'
    $destination=Join-Path $scratch $file.stagePath;[IO.Directory]::CreateDirectory((Split-Path $destination))|Out-Null
    Copy-Item -LiteralPath (Join-Path $SdkDirectory $file.stagePath) -Destination $destination
}
Assert-XessGenerationPayload $scratch $pin
$damage=Join-Path $scratch $pin.generationRuntimes[1].stagePath
[IO.File]::WriteAllText($damage,'not the pinned runtime');Rejected {Assert-XessGenerationPayload $scratch $pin} 'mismatched XeLL rejected'
$ini=Join-Path $scratch 'trial.ini'
$baseline=Join-Path $PSScriptRoot '../package/SKSE/Plugins/RaZkolbaS.ini';$before=(Get-FileHash -LiteralPath $baseline).Hash
$lines=Set-PackageIniValues ([IO.File]::ReadAllLines($baseline)) @{
    'Upscaling/Upscaler'='FSR';'FSR/Quality'='Native';'FSR/Provider'='FSR3';'FSR/SourceColorEncoding'='Gamma22';
    'FrameGeneration/Backend'='XeSS';'FrameGeneration/Enabled'='false';'NeuralRendering/Enabled'='false';'HDROutput/Enabled'='false';
    'DynamicResolution/Enabled'='false';'DynamicResolution/Oscillate'='false';'Interface/NativeUI'='true';'FrameGeneration/UIComposition'='Dedicated'
}
[IO.File]::WriteAllLines($ini,[string[]]$lines);Assert-XessGenerationIni $ini
$bad=Set-PackageIniValues $lines @{'FrameGeneration/Enabled'='true'};[IO.File]::WriteAllLines($ini,[string[]]$bad)
Rejected {Assert-XessGenerationIni $ini} 'unsafe initial enabled trial rejected'
Require ((Get-FileHash -LiteralPath $baseline).Hash -eq $before) 'public baseline INI unchanged'
Write-Output 'PASS independent compiled Intel capability, paired pins, portable Native initial settings and baseline preservation'
