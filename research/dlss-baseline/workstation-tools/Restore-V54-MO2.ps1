$ErrorActionPreference = 'Stop'
if (Get-Process -Name SkyrimSE,ModOrganizer -ErrorAction SilentlyContinue) { throw 'Close Skyrim and MO2 before restoration.' }
$workspace = Split-Path $PSScriptRoot -Parent
$moRoot = [IO.Path]::GetFullPath('D:\TESV54BETA\BETA_TRUEAE_V54')
$profileRoot = Join-Path $moRoot 'profiles'
$modRoot = Join-Path $moRoot 'mods'
$testProfile = Join-Path $profileRoot 'TRP 0.3.5 Standard FG baseline'
$testMod = Join-Path $modRoot 'TRP 0.3.5 Standard - DvaKolbas baseline'
$originalProfile = Join-Path $profileRoot 'V5.4 NO-LORE'
$archive = Join-Path $workspace ('out\mo2-rollback-' + (Get-Date -Format 'yyyyMMdd-HHmmss'))
foreach ($target in $testProfile,$testMod) {
    $resolved = (Resolve-Path -LiteralPath $target).Path
    $expectedParent = if ($target -eq $testProfile) { $profileRoot } else { $modRoot }
    if ([IO.Path]::GetDirectoryName($resolved) -ne $expectedParent) { throw "Unexpected target: $resolved" }
}
if (Test-Path -LiteralPath $archive) { throw 'Rollback archive already exists.' }
$iniPath = Join-Path $moRoot 'ModOrganizer.ini'
$iniText = [IO.File]::ReadAllText($iniPath)
if ($iniText -notmatch '(?m)^selected_profile=@ByteArray\(V5\.4 NO-LORE\)\r?$') { throw 'Original profile is not selected; inspect before restoring.' }
$checks = @()
foreach ($name in 'archives.txt','initweaks.ini','loadorder.txt','lockedorder.txt','plugins.txt','settings.ini','Skyrim.ini','SkyrimCustom.ini','SkyrimPrefs.ini') {
    $originalHash = (Get-FileHash -LiteralPath (Join-Path $originalProfile $name) -Algorithm SHA256).Hash
    $copyHash = (Get-FileHash -LiteralPath (Join-Path $testProfile $name) -Algorithm SHA256).Hash
    $checks += [ordered]@{file=$name; matchesPreTestCopy=($originalHash -eq $copyHash); sha256=$originalHash.ToLowerInvariant()}
}
$expectedModList = [IO.File]::ReadAllText((Join-Path $testProfile 'modlist.txt'))
$expectedModList = [regex]::Replace($expectedModList,'(?m)^[+-]TRP 0\.3\.5 Standard - DvaKolbas baseline\r?\n','')
$expectedModList = [regex]::Replace($expectedModList,'(?m)^-RazKolbas\r?$','+RazKolbas')
New-Item -ItemType Directory -Path $archive -Force | Out-Null
Copy-Item -LiteralPath $iniPath -Destination (Join-Path $archive 'ModOrganizer.ini.before-rollback')
$changedProfiles = @()
foreach ($profile in Get-ChildItem -LiteralPath $profileRoot -Directory) {
    if ($profile.FullName -eq $testProfile) { continue }
    $modListPath = Join-Path $profile.FullName 'modlist.txt'
    if (-not (Test-Path -LiteralPath $modListPath -PathType Leaf)) { continue }
    $text = [IO.File]::ReadAllText($modListPath)
    $restored = [regex]::Replace($text,'(?m)^[+-]TRP 0\.3\.5 Standard - DvaKolbas baseline\r?\n','')
    if ($text -ne $restored) {
        $backup = Join-Path $archive ('profile-backups\' + $profile.Name)
        New-Item -ItemType Directory -Path $backup -Force | Out-Null
        Copy-Item -LiteralPath $modListPath -Destination $backup
        [IO.File]::WriteAllText($modListPath,$restored,[Text.UTF8Encoding]::new($false))
        $changedProfiles += $profile.Name
    }
}
Move-Item -LiteralPath $testProfile -Destination (Join-Path $archive 'test-profile')
Move-Item -LiteralPath $testMod -Destination (Join-Path $archive 'test-mod')
$actualModList = [IO.File]::ReadAllText((Join-Path $originalProfile 'modlist.txt'))
$record = [ordered]@{
    originalSelectedProfile='V5.4 NO-LORE';
    selectedProfileVerified=([IO.File]::ReadAllText($iniPath) -match '(?m)^selected_profile=@ByteArray\(V5\.4 NO-LORE\)\r?$');
    changedProfiles=$changedProfiles;
    originalModListMatchesPreTestCopy=($actualModList -eq $expectedModList);
    originalRazKolbasEnabled=($actualModList -match '(?m)^\+RazKolbas\r?$');
    originalSettingsChecks=$checks;
    testModRemovedFromMO2=(-not (Test-Path -LiteralPath $testMod));
    testProfileRemovedFromMO2=(-not (Test-Path -LiteralPath $testProfile));
    archive=$archive;
    globalIniChangedByRollback=$false;
    gameAcceptance='NOT RUN'
}
$record | ConvertTo-Json -Depth 6 | Tee-Object -FilePath (Join-Path $workspace 'out\validation\mo2-restoration.json')
if (-not $record.originalModListMatchesPreTestCopy -or ($checks | Where-Object { -not $_.matchesPreTestCopy })) { throw 'Unexpected difference from pre-test profile copy; review restoration record.' }
