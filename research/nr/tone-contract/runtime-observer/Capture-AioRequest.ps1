[CmdletBinding()]
param(
    [string]$InstallationRoot='D:/TESV54BETA/BETA_TRUEAE_V54',
    [string]$PythonPath='C:/Python314/python.exe',
    [switch]$Preflight
)
$ErrorActionPreference='Stop'
$repoRoot=(Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '../../../..')).Path
$reader=Join-Path $PSScriptRoot 'ReadAioRequest.py'
$profile=Join-Path $InstallationRoot 'profiles/V5.4 NO-LORE/modlist.txt'
$modRoot=Join-Path $InstallationRoot 'mods/SkyrimUpscalerAIOBuild19-Hotfix1'
$ini=Join-Path $modRoot 'SKSE/Plugins/SkyrimUpscaler.ini'
$modList=Get-Content -LiteralPath $profile
if($modList -notcontains '+SkyrimUpscalerAIOBuild19-Hotfix1' -or
   $modList -notcontains '-TRP NR Before - DvaKolbas trial'){
    throw 'Select only AIO19 in V5.4 NO-LORE before capturing. No profile changes were made.'
}
$pins=@{
    'SKSE/Plugins/SkyrimUpscaler.dll'='ac699b5883d4deeafdfff81ab041c3f191b1970e68c3408ffdb18068a6f5482a'
    'UpscalerBasePlugin/PDPerfPlugin.dll'='ff6c58391501006474e36afaac2b5a5cfceda47850abdbbcc7f2fe5521004435'
    'UpscalerBasePlugin/nvngx_dlssnr.dll'='8270b350cd82de5ce89806872cdd6b6a9249b80836b91bbeb3573470744cc206'
}
foreach($relative in $pins.Keys){
    if((Get-FileHash -LiteralPath (Join-Path $modRoot $relative) -Algorithm SHA256).Hash.ToLowerInvariant() -ne $pins[$relative]){
        throw "Unqualified installed binary: $relative"
    }
}
if(-not(Test-Path -LiteralPath $PythonPath -PathType Leaf)){throw 'Python runtime missing.'}
& $PythonPath -c 'import pefile'
if($LASTEXITCODE -ne 0){throw 'Python pefile dependency missing.'}
function Read-SelectedSettings {
    $result=[ordered]@{}
    $section=''
    foreach($line in Get-Content -LiteralPath $ini){
        $text=$line.Trim()
        if($text -match '^\[([^\]]+)\]$'){$section=$Matches[1];continue}
        if($section -notin @('Settings','Frame Generation','DLSS NR','NR PASS 1') -or
           $text.StartsWith(';') -or $text.StartsWith('#')){continue}
        if($text -match '^([^=]+)=(.*)$'){
            if(-not $result.Contains($section)){$result[$section]=[ordered]@{}}
            $result[$section][$Matches[1].Trim()]=$Matches[2].Trim()
        }
    }
    return $result
}
if($Preflight){
    Write-Output 'PREFLIGHT PASS: AIO19 binaries pinned; profile selects only AIO; Python dependency available.'
    Write-Output 'No game, files or settings changed. Load the save, enable NR Style 0 / Tone 1 and leave FG off.'
    return
}
$games=@(Get-Process SkyrimSE -ErrorAction SilentlyContinue)
if($games.Count -ne 1){throw 'Start one Skyrim instance and load the save before capturing.'}
$principal=[Security.Principal.WindowsPrincipal]::new([Security.Principal.WindowsIdentity]::GetCurrent())
if(-not $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)){
    throw 'This installation required elevated query/read access. Run Capture-AIO19-NR.cmd as administrator.'
}
$stamp=[DateTime]::UtcNow.ToString('yyyyMMdd-HHmmss')+'-AIO19-passive-'+[Guid]::NewGuid().ToString('N').Substring(0,8)
$outputRoot=Join-Path $repoRoot ('out/research/nr/tone-boundary-game/'+$stamp)
New-Item -ItemType Directory -Path $outputRoot | Out-Null
$receipt=[ordered]@{
    label='AIO19';pid=$games[0].Id;startedUtc=[DateTime]::UtcNow.ToString('o')
    wrapperSha256=(Get-FileHash -LiteralPath $PSCommandPath -Algorithm SHA256).Hash.ToLowerInvariant()
    installedPins=$pins
    iniBeforeSha256=(Get-FileHash -LiteralPath $ini -Algorithm SHA256).Hash.ToLowerInvariant()
    profileBeforeSha256=(Get-FileHash -LiteralPath $profile -Algorithm SHA256).Hash.ToLowerInvariant()
    settingsBefore=(Read-SelectedSettings)
    settingsEditedByScript=$false;capture='runtime.json'
}
Write-Output 'Rotate around the building for 15 seconds. This capture only reads retained requests and qualified texture metadata.'
$toolExit=-1
try{
    & $PythonPath $reader --pid $games[0].Id --output (Join-Path $outputRoot 'runtime.json') --samples 15
    $toolExit=$LASTEXITCODE
}finally{
    $receipt['readerExitCode']=$toolExit
    $receipt['finishedUtc']=[DateTime]::UtcNow.ToString('o')
    $receipt['iniAfterSha256']=(Get-FileHash -LiteralPath $ini -Algorithm SHA256).Hash.ToLowerInvariant()
    $receipt['profileAfterSha256']=(Get-FileHash -LiteralPath $profile -Algorithm SHA256).Hash.ToLowerInvariant()
    $receipt['settingsAfter']=Read-SelectedSettings
    $receipt | ConvertTo-Json -Depth 10 | Set-Content -LiteralPath (Join-Path $outputRoot 'session.json') -Encoding UTF8
    Write-Output "Capture saved: $outputRoot"
}
if($toolExit -ne 0){throw "Reader failed with code $toolExit. Inspect capture output; no settings were edited."}
$data=Get-Content -LiteralPath (Join-Path $outputRoot 'runtime.json') -Raw | ConvertFrom-Json
Write-Output "FINISHED: $($data.acceptedSamples) retained request snapshots. Texture statuses must be checked separately."
$rows=foreach($sample in $data.samples){
    foreach($resource in @('color','motion','depth','output')){
        $metadata=$sample.textureMetadata.$resource
        if($null -ne $metadata){
            [pscustomobject]@{resource=$resource;status=$metadata.status;reason=$metadata.reason
                width=$metadata.width;height=$metadata.height;format=$metadata.format}
        }
    }
}
$rows | Select-Object resource,status,reason,width,height,format -Unique | Format-Table
