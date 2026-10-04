[CmdletBinding()]
param(
    [Parameter(Mandatory=$true)][ValidateSet('AIO19','Dva')][string]$Label,
    [string]$InstallationRoot='D:/TESV54BETA/BETA_TRUEAE_V54',
    [switch]$Preflight
)
$ErrorActionPreference='Stop'
$repoRoot=(Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '../../../..')).Path
$observer=Join-Path $repoRoot 'out/build/nr-boundary-observer/Release/NrBoundaryObserver.exe'
if(-not(Test-Path -LiteralPath $observer -PathType Leaf)){throw 'Build NrBoundaryObserver first; see README.md'}
$profile=Join-Path $InstallationRoot 'profiles/V5.4 NO-LORE/modlist.txt'
$modList=Get-Content -LiteralPath $profile
if($Label -eq 'AIO19'){
    $modName='SkyrimUpscalerAIOBuild19-Hotfix1'
    $otherMod='TRP NR Before - DvaKolbas trial'
    $plugin='SKSE/Plugins/SkyrimUpscaler.dll'
    $pluginHash='ac699b5883d4deeafdfff81ab041c3f191b1970e68c3408ffdb18068a6f5482a'
    $iniRelative='SKSE/Plugins/SkyrimUpscaler.ini'
    $model='UpscalerBasePlugin/nvngx_dlssnr.dll'
    $modelHash='8270b350cd82de5ce89806872cdd6b6a9249b80836b91bbeb3573470744cc206'
    $sections=@('Settings','Frame Generation','DLSS NR','NR PASS 1')
}else{
    $modName='TRP NR Before - DvaKolbas trial'
    $otherMod='SkyrimUpscalerAIOBuild19-Hotfix1'
    $plugin='SKSE/Plugins/TheosRenderPipeline.dll'
    $pluginHash='f829ad80fe92d195c7c707368909f077aae6fbec87b94ac0687be79eb8904058'
    $iniRelative='SKSE/Plugins/TheosRenderPipeline.ini'
    $model='SKSE/Plugins/TheosRenderPipeline/NR/rtx40/nvngx_dlssnr.dll'
    $modelHash='e67dee209320cdafe0e93e45675d7aa34323a53acc57a72b2e40a181581c989a'
    $sections=@('Settings','Experimental','SourceDLSSG','Compatibility')
}
if($modList -notcontains ('+'+$modName) -or $modList -notcontains ('-'+$otherMod)){
    throw "Select $modName and disable $otherMod in V5.4 NO-LORE before this capture. No profile changes were made."
}
$modRoot=Join-Path $InstallationRoot ('mods/'+$modName)
$ini=Join-Path $modRoot $iniRelative
foreach($pair in @(@($plugin,$pluginHash),@($model,$modelHash))){
    if((Get-FileHash -LiteralPath (Join-Path $modRoot $pair[0]) -Algorithm SHA256).Hash.ToLowerInvariant() -ne $pair[1]){
        throw "Unqualified installed binary: $($pair[0])"
    }
}
function Read-SelectedSettings([string]$Path){
    $result=[ordered]@{}
    $current=''
    foreach($line in Get-Content -LiteralPath $Path){
        $text=$line.Trim()
        if($text -match '^\[([^\]]+)\]$'){$current=$Matches[1];continue}
        if($current -notin $sections -or $text.StartsWith(';') -or $text.StartsWith('#')){continue}
        if($text -match '^([^=]+)=(.*)$'){
            if(-not $result.Contains($current)){$result[$current]=[ordered]@{}}
            $result[$current][$Matches[1].Trim()]=$Matches[2].Trim()
        }
    }
    # Authentication sections are deliberately not serialized.
    return $result
}
$beforeSettings=Read-SelectedSettings $ini
$beforeIniHash=(Get-FileHash -LiteralPath $ini -Algorithm SHA256).Hash.ToLowerInvariant()
if($Preflight){
    Write-Output "PREFLIGHT PASS: $Label installed binaries pinned; profile selects only this host."
    Write-Output 'No files or settings changed. Load the building scene, enable NR Style 0 / Tone 1, keep FG off, then run without -Preflight.'
    return
}
$games=@(Get-Process SkyrimSE -ErrorAction SilentlyContinue)
if($games.Count -ne 1){throw 'Start one Skyrim instance and load the save before capturing.'}
$stamp=[DateTime]::UtcNow.ToString('yyyyMMdd-HHmmss')+'-'+$Label+'-'+[Guid]::NewGuid().ToString('N').Substring(0,8)
$outputRoot=Join-Path $repoRoot ('out/research/nr/tone-boundary-game/'+$stamp)
New-Item -ItemType Directory -Path $outputRoot | Out-Null
$capture=Join-Path $outputRoot 'runtime.json'
$cancel=Join-Path $outputRoot 'cancel'
Write-Output 'Rotate around the same building during the capture. This diagnostic pauses briefly at NR dispatches; it is not a performance test.'
Write-Output "It stops automatically. To request early cleanup, create this file: $cancel"
& $observer --pid $games[0].Id --output $capture --seconds 15 --samples 32 --stride 15 --cancel-file $cancel
$toolExit=$LASTEXITCODE
$receipt=[ordered]@{
    label=$Label;capturedUtc=[DateTime]::UtcNow.ToString('o');pid=$games[0].Id
    observerSha256=(Get-FileHash -LiteralPath $observer -Algorithm SHA256).Hash.ToLowerInvariant()
    installedPluginSha256=$pluginHash;installedModelSha256=$modelHash
    profileModListSha256=(Get-FileHash -LiteralPath $profile -Algorithm SHA256).Hash.ToLowerInvariant()
    iniBeforeSha256=$beforeIniHash;iniAfterSha256=(Get-FileHash -LiteralPath $ini -Algorithm SHA256).Hash.ToLowerInvariant()
    settingsBefore=$beforeSettings;settingsAfter=(Read-SelectedSettings $ini);observerExitCode=$toolExit
    settingsEditedByScript=$false;capture='runtime.json'
}
$receipt | ConvertTo-Json -Depth 10 | Set-Content -LiteralPath (Join-Path $outputRoot 'session.json') -Encoding UTF8
if(Test-Path -LiteralPath $capture){
    $data=Get-Content -LiteralPath $capture -Raw | ConvertFrom-Json
    if($data.moduleSha256 -ne $modelHash){throw 'Actual loaded NR image differs from the selected host; capture retained for diagnosis.'}
    if($data.samples.Count){
        $data.samples | Select-Object -Property style,tone,reset,enabled,autoMask -Unique | Format-Table
    }
}
Write-Output "Capture saved: $outputRoot"
if($toolExit -ne 0){throw "Observer finished with code $toolExit; inspect runtime.json/session.json. No mod settings were edited."}
