$ErrorActionPreference = 'Stop'
$workspace = Split-Path $PSScriptRoot -Parent
$moRoot = 'D:\TESV54BETA\BETA_TRUEAE_V54'
$sourceProfile = Join-Path $moRoot 'profiles\V5.4 NO-LORE'
$profileName = 'TRP 0.3.5 Standard FG baseline'
$modName = 'TRP 0.3.5 Standard - DvaKolbas baseline'
$profileDestination = Join-Path $moRoot "profiles\$profileName"
$modDestination = Join-Path $moRoot "mods\$modName"
$package = Join-Path $workspace 'out\packages\TRP-0.3.5-standard-246d152c-dlss310.8-streamline2.13'
if (Get-Process -Name SkyrimSE,ModOrganizer -ErrorAction SilentlyContinue) { throw 'Close Skyrim and MO2 before preparing the separate test profile.' }
foreach ($destination in $profileDestination,$modDestination) {
    if (Test-Path -LiteralPath $destination) { throw "Refusing to overwrite existing destination: $destination" }
}
if (-not (Test-Path -LiteralPath (Join-Path $package 'SKSE\Plugins\TheosRenderPipeline.dll'))) { throw 'Staged Standard package is missing.' }
New-Item -ItemType Directory -Path $modDestination,$profileDestination -Force | Out-Null
Get-ChildItem -LiteralPath $package -Force | Copy-Item -Destination $modDestination -Recurse
foreach ($profileFile in 'archives.txt','initweaks.ini','loadorder.txt','lockedorder.txt','modlist.txt','plugins.txt','settings.ini','Skyrim.ini','SkyrimCustom.ini','SkyrimPrefs.ini') {
    $profileSourceFile = Join-Path $sourceProfile $profileFile
    if (Test-Path -LiteralPath $profileSourceFile -PathType Leaf) { Copy-Item -LiteralPath $profileSourceFile -Destination $profileDestination }
}
$modListPath = Join-Path $profileDestination 'modlist.txt'
$modList = [IO.File]::ReadAllText($modListPath)
if ($modList -notmatch '(?m)^\+RazKolbas\r?$') { throw 'Expected active RazKolbas renderer was not found in the source profile.' }
$modList = [regex]::Replace($modList,'(?m)^\+RazKolbas\r?$','-RazKolbas')
$firstLineBreak = $modList.IndexOf("`n")
if ($firstLineBreak -lt 0) { throw 'Source modlist has no header line.' }
$modList = $modList.Insert($firstLineBreak + 1, "+$modName`r`n")
[IO.File]::WriteAllText($modListPath,$modList,[Text.UTF8Encoding]::new($false))
$iniPath = Join-Path $modDestination 'SKSE\Plugins\TheosRenderPipeline.ini'
$ini = [IO.File]::ReadAllText($iniPath)
$fgSetting = [regex]::new('(?ms)(^\[FrameGeneration\]\r?\n.*?^Enabled=)true(?=\r?$)')
if ($fgSetting.Matches($ini).Count -ne 1) { throw 'Expected one frame-generation Enabled setting.' }
$ini = $fgSetting.Replace($ini,'${1}false',1)
[IO.File]::WriteAllText($iniPath,$ini,[Text.UTF8Encoding]::new($false))
$saveDirectory = Join-Path $profileDestination 'saves'
New-Item -ItemType Directory -Path $saveDirectory -Force | Out-Null
$latestSave = Get-ChildItem -LiteralPath (Join-Path $sourceProfile 'saves') -File -Filter '*.ess' | Sort-Object LastWriteTime -Descending | Select-Object -First 1
$copiedSaveFiles = @()
if ($latestSave) {
    foreach ($extension in '.ess','.skse') {
        $sourceSave = Join-Path $latestSave.DirectoryName ($latestSave.BaseName + $extension)
        if (Test-Path -LiteralPath $sourceSave -PathType Leaf) {
            Copy-Item -LiteralPath $sourceSave -Destination $saveDirectory
            $copiedSaveFiles += [IO.Path]::GetFileName($sourceSave)
        }
    }
}
[IO.File]::WriteAllText((Join-Path $modDestination 'meta.ini'),"[General]`r`nnotes=Local TRP 0.3.5 Standard build from 246d152c. DLSS-G runtime 310.8.0 and Streamline 2.13 supplied by user. FG initially off; NR and HDR off. Runtime acceptance pending.`r`n",[Text.UTF8Encoding]::new($false))
$record = [ordered]@{ profile=$profileName; profilePath=$profileDestination; mod=$modName; modPath=$modDestination; sourceProfile='V5.4 NO-LORE'; initialFG='OFF: verify DLSS first, then enable native x2 using End / Apply'; sourceProfileChanged=$false; selectedProfileChanged=$false; copiedSaves=$copiedSaveFiles; pluginSha256=(Get-FileHash -LiteralPath (Join-Path $modDestination 'SKSE\Plugins\TheosRenderPipeline.dll') -Algorithm SHA256).Hash.ToLowerInvariant(); gameAcceptance='NOT RUN' }
$record | ConvertTo-Json -Depth 4 | Tee-Object -FilePath (Join-Path $workspace 'out\validation\v54-test-profile.json')
