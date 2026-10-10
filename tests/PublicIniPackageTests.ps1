param([Parameter(Mandatory)][string]$Repository,[Parameter(Mandatory)][string]$Output)
$ErrorActionPreference='Stop'
. (Join-Path $Repository 'tools/fsr/PackageCommon.ps1')
[IO.Directory]::CreateDirectory($Output)|Out-Null
$template=Join-Path $Repository 'package/SKSE/Plugins/RaZkolbaS.ini'
$view=Read-PackageIni $template
$defaults=Read-PackageIni $template -Raw
if($defaults['FrameGeneration/Backend'] -ne 'Auto' -or
   $defaults['FrameGeneration/Enabled'] -ne 'false' -or
   $defaults['NeuralRendering/Enabled'] -ne 'false') {
    throw 'Release defaults must use Auto presentation with FG and NR disabled'
}
foreach($upscaler in @('DLSS','FSR','XeSS')) {
    foreach($backend in @('Auto','NVIDIA','FSR','XeSS')) {
        $roundtrip = Set-PackageIniValues @('[Upscaling]',('Upscaler='+$upscaler),'[FrameGeneration]',('Backend='+$backend)) @{}
        $roundtripPath = Join-Path $Output ($upscaler+'-'+$backend+'.ini')
        [IO.File]::WriteAllLines($roundtripPath,[string[]]$roundtrip)
        $expected = if($backend -eq 'XeSS'){'3'}elseif($backend -eq 'FSR'){'2'}elseif($upscaler -eq 'XeSS'){'0'}elseif($backend -eq 'NVIDIA'){'1'}elseif($upscaler -eq 'FSR'){'2'}else{'1'}
        $savedPreference=if($upscaler -eq 'XeSS' -and $backend -eq 'NVIDIA'){'Auto'}else{$backend}
        if((Read-PackageIni $roundtripPath -Raw)['FrameGeneration/Backend'] -ne $savedPreference -or
           (Read-PackageIni $roundtripPath)['Experimental/FrameGenerationBackend'] -ne $expected) {
            throw 'Package round-trip must preserve independent FG preference, including Auto'
        }
    }
}
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
$text=[IO.File]::ReadAllText($target)
if(([regex]::Matches($text,'(?m)^; Whole numbers only\.\r?$')).Count -ne 12){
    throw 'All 12 integer controls must document their whole-number restriction on a separate line'
}
foreach($choices in @('Values: Native | Quality | Balanced | Performance | UltraPerformance | UltraQuality.',
    'Values: Native | Quality | Balanced | Performance.', 'Values: FSR3 | Auto | FSR4.',
    'Values: Default | E | F | J | K | L | M.', 'Values: Off | On | Boost.',
    'Values: Before | After.',
    'Values: Unknown | Linear | Gamma22 | SRGB.', 'Values: Auto | rtx20-30 | rtx40 | rtx50.',
    'Values: Auto | Residual | Ratio.', 'Values: true | false.')){
    if(-not $text.Contains($choices)){throw "Converted INI must list allowed choices beside the setting: $choices"}
}
$raw=Read-PackageIni $target -Raw
if($raw['Upscaling/Upscaler'] -ne 'FSR' -or $raw['DLSS/Quality'] -ne 'Native' -or
   $raw['FSR/Provider'] -ne 'FSR4' -or $raw['FrameGeneration/FsrProvider'] -ne 'FSR4' -or
   $raw['NeuralRendering/Placement'] -ne 'After' -or $raw['Debug/NRLegacyRuntime'] -ne 'false' -or
   $raw.ContainsKey('NeuralRendering Advanced/Runtime') -or
   $raw.ContainsKey('Settings/UpscaleType') -or $raw['FrameGeneration/Backend'] -ne 'Auto'){
    throw 'Packaging must emit only named settings, explicit ML and configured Auto backend'
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
    @('[Upscaling]','Upscaler=DLSS','[FrameGeneration]','Backend=Ordinary'),
    @('[Upscaling]','Upscaler=FSR','MipLodBias=banana'),
    @('[Upscaling]','Upscaler=FSR','[FrameGeneration]','Enabled=maybe'),
    @('[Upscaling]','Upscaler=FSR','[Hotkeys]','ToggleOverlay=0'),
    @('[Upscaling]','Upscaler=DLSS','[NR PASS 1]','Style=1.0'),
    @('[Upscaling]','Upscaler=DLSS','[FrameGeneration]','NvidiaGeneratedFrames=2.0'),
    @('[Upscaling]','Upscaler=DLSS','[FSR]','SourceColorEncoding=Gama22'),
    @('[Upscaling]','Upscaler=DLSS','[NeuralRendering Advanced]','Profile=rtx41'),
    @('[Upscaling]','Upscaler=DLSS','[Settings]','EnableUpscaler=false'),
    @('[Upscaling]','Upscaler=FSR','[FrameGeneration]','Enabled=true','[Upscaling Advanced]','FsrOrdinaryPresenter=true'))){
    $rejected=$false;try{Set-PackageIniValues $invalid @{} | Out-Null}catch{$rejected=$true}
    if(-not $rejected){throw 'Package decoder must reject the same invalid settings as runtime'}
}
foreach($name in @('Home','PageUp','PageDown','Delete','Tab')){
    $named=Set-PackageIniValues @('[Upscaling]','Upscaler=DLSS','[Hotkeys]',('ToggleOverlay='+$name)) @{}
    if(-not ($named -contains ('ToggleOverlay = '+$name))){throw "Named hotkey lost: $name"}
}
$xessLines=Set-PackageIniValues $nativeLines @{'Upscaling/Upscaler'='XeSS';'FrameGeneration/Enabled'='false';'XeSS/Quality'='Native'}
$xessPath=Join-Path $Output 'xess.ini';[IO.File]::WriteAllLines($xessPath,[string[]]$xessLines)
$xess=Read-PackageIni $xessPath -Raw
if($xess['Upscaling/Upscaler'] -ne 'XeSS' -or $xess['DLSS/Quality'] -ne 'Native') {throw 'XeSS packaging must retain provider and inactive DLAA preference'}
if((Read-PackageIni $xessPath)['FrameGeneration/Backend'] -ne '0') {throw 'XeSS SR trial must select ordinary presentation'}
$xessNvLines=Set-PackageIniValues $xessLines @{'FrameGeneration/Backend'='NVIDIA';'FrameGeneration/Enabled'='true'}
$xessNvPath=Join-Path $Output 'xess-normalized.ini';[IO.File]::WriteAllLines($xessNvPath,[string[]]$xessNvLines)
$xessNv=Read-PackageIni $xessNvPath
if($xessNv['FrameGeneration/Backend'] -ne '0' -or $xessNv['FrameGeneration/Enabled'] -ne 'false'){
    throw 'XeSS unavailable NVIDIA FG must normalize to SR-only, matching the runtime decoder and menu'
}
$xessFgLines=Set-PackageIniValues $xessLines @{'FrameGeneration/Backend'='FSR';'FrameGeneration/Enabled'='true'}
$xessFgPath=Join-Path $Output 'xess-fsr-fg.ini';[IO.File]::WriteAllLines($xessFgPath,[string[]]$xessFgLines)
if((Read-PackageIni $xessFgPath)['FrameGeneration/Backend'] -ne '2'){throw 'Explicit XeSS FSR FG must select FSR presentation'}
Write-Output 'PASS: packaging and runtime schema agree; Native/ML/derived FG and unknown comments preserved'
