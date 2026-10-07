param([Parameter(Mandatory)][string]$Repository,[Parameter(Mandatory)][string]$Output)
$ErrorActionPreference='Stop'
. (Join-Path $Repository 'tools/fsr/PackageCommon.ps1')
[IO.Directory]::CreateDirectory($Output)|Out-Null
$template=Join-Path $Repository 'package/SKSE/Plugins/RaZkolbaS.ini'
$view=Read-PackageIni $template
if($view['Settings/UpscaleType'] -ne '3' -or $view['Settings/DLSSPreset'] -ne '11' -or
   $view['Experimental/FrameGenerationBackend'] -ne '1' -or $view['FSR/Quality'] -ne 'NativeAA'){
    throw 'Package decoder must match runtime Native DLSS/FSR and derived presenter'
}
$lines=@('; Keep this global note.','GlobalUserOption = custom')+@([IO.File]::ReadAllLines($template))+@('','[User Extras]','; Keep this user note.','Value = untouched')
$changed=Set-PackageIniValues $lines @{
    'Settings/UpscaleType'='4';'FSR/ProviderPolicy'='MachineLearning';
    'FrameGeneration/FsrProviderPolicy'='MachineLearning';'FrameGeneration/Enabled'='false';
    'NeuralRendering/BeforeUpscaling'='false';'NeuralRendering/CommunityRuntime'='true'
}
$target=Join-Path $Output 'named.ini';[IO.File]::WriteAllLines($target,[string[]]$changed)
$raw=Read-PackageIni $target -Raw
if($raw['Upscaling/Upscaler'] -ne 'FSR' -or $raw['DLSS/Quality'] -ne 'Native' -or
   $raw['FSR/Provider'] -ne 'FSR4' -or $raw['FrameGeneration/FsrProvider'] -ne 'FSR4' -or
   $raw['NeuralRendering/Placement'] -ne 'After' -or $raw['NeuralRendering Advanced/Runtime'] -ne 'Community' -or
   $raw.ContainsKey('Settings/UpscaleType') -or $raw.ContainsKey('FrameGeneration/Backend')){
    throw 'Packaging must emit only named settings, explicit ML and no derived backend'
}
$loaded=Read-PackageIni $target
if($loaded['Settings/UpscaleType'] -ne '4' -or $loaded['Experimental/FrameGenerationBackend'] -ne '2' -or
   $loaded['FSR/ProviderPolicy'] -ne 'MachineLearning' -or $loaded['FrameGeneration/FsrProviderPolicy'] -ne 'MachineLearning'){
    throw 'The named package must reload with explicit ML policies and a live-ready FSR presenter'
}
if([IO.File]::ReadAllText($target) -notmatch 'Keep this user note' -or $raw['User Extras/Value'] -ne 'untouched'){
    throw 'Packaging must preserve unknown settings and comments'
}
if($raw['/GlobalUserOption'] -ne 'custom' -or [IO.File]::ReadAllText($target) -notmatch 'Keep this global note'){
    throw 'Global unknown settings and comments must also survive packaging'
}
$qualityLines=Set-PackageIniValues $lines @{'DLSS/Quality'='Quality'}
$qualityPath=Join-Path $Output 'quality.ini';[IO.File]::WriteAllLines($qualityPath,[string[]]$qualityLines)
if((Read-PackageIni $qualityPath -Raw)['DLSS/Quality'] -ne 'Quality'){
    throw 'Public Quality selector must disable the independent DLSS Native preference'
}
$nativeLines=Set-PackageIniValues $qualityLines @{'DLSS/Quality'='Native'}
$nativePath=Join-Path $Output 'native.ini';[IO.File]::WriteAllLines($nativePath,[string[]]$nativeLines)
if((Read-PackageIni $nativePath -Raw)['DLSS/Quality'] -ne 'Native'){
    throw 'Public Native selector must update the independent DLSS Native preference'
}
foreach($invalid in @(
    @('[Upscaling]','Upscaler=FSR','MipLodBias=banana'),
    @('[Upscaling]','Upscaler=FSR','[FrameGeneration]','Enabled=maybe'),
    @('[Upscaling]','Upscaler=FSR','[Hotkeys]','ToggleOverlay=0'),
    @('[Upscaling]','Upscaler=FSR','[FrameGeneration]','Enabled=true','[Upscaling Advanced]','FsrOrdinaryPresenter=true'))){
    $rejected=$false;try{Set-PackageIniValues $invalid @{} | Out-Null}catch{$rejected=$true}
    if(-not $rejected){throw 'Package decoder must reject the same invalid settings as runtime'}
}
Write-Output 'PASS: packaging and runtime schema agree; Native/ML/derived FG and unknown comments preserved'
