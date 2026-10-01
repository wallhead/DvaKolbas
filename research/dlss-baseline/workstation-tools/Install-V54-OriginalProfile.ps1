$ErrorActionPreference = 'Stop'
if (Get-Process -Name SkyrimSE,ModOrganizer -ErrorAction SilentlyContinue) { throw 'Close Skyrim and MO2 before installing.' }
$workspace = Split-Path $PSScriptRoot -Parent
$moRoot = 'D:\TESV54BETA\BETA_TRUEAE_V54'
$profile = Join-Path $moRoot 'profiles\V5.4 NO-LORE'
$modName = 'TRP 0.3.5 Standard - DvaKolbas baseline'
$destination = Join-Path $moRoot "mods\$modName"
$package = Join-Path $workspace 'out\packages\TRP-0.3.5-standard-246d152c-dlss310.8-streamline2.13'
$modListPath = Join-Path $profile 'modlist.txt'
$modList = [IO.File]::ReadAllText($modListPath)
if (Test-Path -LiteralPath $destination) { throw 'TRP destination already exists; inspect before overwriting.' }
if ($modList -notmatch '(?m)^[+-]RazKolbas\r?$') { throw 'Expected RazKolbas entry not found.' }
if ($modList -match '(?m)^[+-]TRP 0\.3\.5 Standard - DvaKolbas baseline\r?$') { throw 'TRP is already listed in this profile.' }
$manifest = Get-Content -LiteralPath (Join-Path $package 'build-manifest.json') -Raw | ConvertFrom-Json
foreach ($file in $manifest.files) {
    $hash = (Get-FileHash -LiteralPath (Join-Path $package $file.path) -Algorithm SHA256).Hash.ToLowerInvariant()
    if ($hash -ne $file.sha256) { throw "Package hash mismatch: $($file.path)" }
}
$settingsBefore = @{}
foreach ($name in 'archives.txt','initweaks.ini','loadorder.txt','lockedorder.txt','plugins.txt','settings.ini','Skyrim.ini','SkyrimCustom.ini','SkyrimPrefs.ini') {
    $settingsBefore[$name] = (Get-FileHash -LiteralPath (Join-Path $profile $name) -Algorithm SHA256).Hash
}
$backup = Join-Path $workspace ('out\v54-install-backup-' + (Get-Date -Format 'yyyyMMdd-HHmmss'))
New-Item -ItemType Directory -Path $backup -Force | Out-Null
Copy-Item -LiteralPath $modListPath -Destination $backup
Copy-Item -LiteralPath (Join-Path $moRoot 'ModOrganizer.ini') -Destination $backup
New-Item -ItemType Directory -Path $destination -Force | Out-Null
Get-ChildItem -LiteralPath $package -Force | Copy-Item -Destination $destination -Recurse
$iniPath = Join-Path $destination 'SKSE\Plugins\TheosRenderPipeline.ini'
$ini = [IO.File]::ReadAllText($iniPath)
$fgSetting = [regex]::new('(?ms)(^\[FrameGeneration\]\r?\n.*?^Enabled=)true(?=\r?$)')
if ($fgSetting.Matches($ini).Count -ne 1) { throw 'Expected one initial FG setting.' }
$ini = $fgSetting.Replace($ini,'${1}false',1)
[IO.File]::WriteAllText($iniPath,$ini,[Text.UTF8Encoding]::new($false))
$updated = [regex]::Replace($modList,'(?m)^\+RazKolbas(?=\r?$)','-RazKolbas')
$firstLineBreak = $updated.IndexOf("`n")
if ($firstLineBreak -lt 0) { throw 'Missing MO2 modlist header.' }
$updated = $updated.Insert($firstLineBreak + 1,"+$modName`r`n")
[IO.File]::WriteAllText($modListPath,$updated,[Text.UTF8Encoding]::new($false))
[IO.File]::WriteAllText((Join-Path $destination 'meta.ini'),"[General]`r`nnotes=TRP Standard 0.3.5, revision 246d152c. User DLSS 310.8 / Streamline 2.13 bundle. FG initially OFF. Skyrim acceptance pending.`r`n",[Text.UTF8Encoding]::new($false))
foreach ($file in $manifest.files) {
    $installedFile = Get-Item -LiteralPath (Join-Path $destination $file.path)
    $file.bytes = $installedFile.Length
    $file.sha256 = (Get-FileHash -LiteralPath $installedFile.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
}
$manifest | Add-Member -NotePropertyName localOverrides -NotePropertyValue @{profile='V5.4 NO-LORE';FrameGenerationEnabled=$false} -Force
$manifest | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath (Join-Path $destination 'build-manifest.json') -Encoding utf8
$settingsChecks = @()
foreach ($name in $settingsBefore.Keys) {
    $same = ((Get-FileHash -LiteralPath (Join-Path $profile $name) -Algorithm SHA256).Hash -eq $settingsBefore[$name])
    if (-not $same) { throw "Unexpected profile settings change: $name" }
    $settingsChecks += [ordered]@{file=$name;unchanged=$same}
}
$installedModList = [IO.File]::ReadAllText($modListPath)
if ($installedModList -notmatch '(?m)^\+TRP 0\.3\.5 Standard - DvaKolbas baseline\r?$' -or $installedModList -notmatch '(?m)^-RazKolbas\r?$') { throw 'Renderer ownership verification failed.' }
$record = [ordered]@{profile='V5.4 NO-LORE';mod=$modName;modPath=$destination;trpEnabled=$true;razKolbasEnabled=$false;initialFG='OFF';requestedFG='native x2 after DLSS check';nr='OFF';hdr='OFF';runtime='DLSS 310.8.0 / Streamline 2.13 supplied by user';backup=$backup;settingsChecks=$settingsChecks;verifiedPackageFiles=$manifest.files.Count;gameAcceptance='NOT RUN'}
$record | ConvertTo-Json -Depth 6 | Tee-Object -FilePath (Join-Path $workspace 'out\validation\v54-original-profile-install.json')
