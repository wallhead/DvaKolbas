param([string]$PresentMonPath,[switch]$Preflight)
$ErrorActionPreference='Stop'
$repository=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
if(-not $PresentMonPath){$PresentMonPath=Join-Path $repository 'out/research/presentmon/PresentMon-2.6.0-x64.exe'}
$PresentMonPath=[IO.Path]::GetFullPath($PresentMonPath)
$expectedHash='b2a706bc6ad475749e3b7e3409263aa1e6906d45bdcf993f6dbc0f660188f1af'
if((Get-FileHash -LiteralPath $PresentMonPath -Algorithm SHA256).Hash.ToLowerInvariant() -ne $expectedHash){throw 'PresentMon binary differs from the verified Intel-signed 2.6.0 binary'}
$signature=Get-AuthenticodeSignature -LiteralPath $PresentMonPath
if($signature.Status -ne 'Valid' -or $signature.SignerCertificate.Subject -notmatch 'CN=Intel Corporation'){throw 'Expected valid Intel signature'}
$identity=[Security.Principal.WindowsIdentity]::GetCurrent()
$administrator=([Security.Principal.WindowsPrincipal]::new($identity)).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
$games=@(Get-Process SkyrimSE -ErrorAction SilentlyContinue)
if($Preflight){
    [ordered]@{binaryVerified=$true;version='2.6.0';administrator=$administrator;skyrimProcesses=$games.Count;ready=($administrator -and $games.Count -eq 1);changesGameOrMo2Settings=$false}|ConvertTo-Json
    exit 0
}
if(-not $administrator){throw 'Right-click Start-Skyrim-FG-Capture.cmd and choose Run as administrator. The local ETW probe returned access denied without elevation.'}
if($games.Count -ne 1){throw 'Start Skyrim manually through MO2, load a save, then run this capture launcher'}
$game=$games[0]
$run=Join-Path $repository ('out/research/skyrim-presentation/'+(Get-Date -Format 'yyyyMMdd-HHmmss')+'-'+[guid]::NewGuid().ToString('N'))
[IO.Directory]::CreateDirectory($run)|Out-Null
$log=Join-Path ([Environment]::GetFolderPath('MyDocuments')) 'My Games/Skyrim Special Edition/SKSE/RaZkolbaS.log'
if(Test-Path -LiteralPath $log){Copy-Item -LiteralPath $log -Destination (Join-Path $run 'skyrim-before.log')}
$captures=@()
Write-Host 'Keep the same scene and camera. Do not Save as default. This tool does not launch Skyrim or edit settings.'
foreach($mode in @('Off','On')){
    $null=Read-Host ('In Skyrim set FG '+$mode+', close the overlay, then return here and press Enter')
    $game.Refresh()
    if($game.HasExited){throw 'Skyrim exited before capture'}
    $csv=Join-Path $run ('fg-'+$mode.ToLowerInvariant()+'.csv')
    $stdout=Join-Path $run ('fg-'+$mode.ToLowerInvariant()+'-stdout.log')
    $stderr=Join-Path $run ('fg-'+$mode.ToLowerInvariant()+'-stderr.log')
    $session='DvaKolbas-'+[guid]::NewGuid().ToString('N')
    $arguments=@('--process_id',[string]$game.Id,'--session_name',$session,'--delay','5','--timed','30','--terminate_after_timed','--no_console_stats','--no_track_input','--date_time','--write_display_metadata','--output_file',('"'+$csv+'"'))
    Write-Host 'Return to Skyrim within 5 seconds; keep its window foreground for 30 seconds, then return here.'
    $started=[datetime]::UtcNow.ToString('o')
    $capture=Start-Process -FilePath $PresentMonPath -ArgumentList $arguments -WindowStyle Hidden -PassThru -Wait -RedirectStandardOutput $stdout -RedirectStandardError $stderr
    if($capture.ExitCode -ne 0){throw ('PresentMon failed; inspect '+$stderr)}
    if(-not (Test-Path -LiteralPath $csv)){throw 'PresentMon produced no CSV'}
    $rows=@(Import-Csv -LiteralPath $csv)
    if(-not $rows.Count){throw 'CSV has no frame records; repeat with the game foreground'}
    $captures+=[ordered]@{requestedMode=$mode;stateSource='User confirmation; verify against Skyrim log';processId=$game.Id;startedUtc=$started;delaySeconds=5;captureSeconds=30;rows=$rows.Count;csv=[IO.Path]::GetFileName($csv);sha256=(Get-FileHash -LiteralPath $csv -Algorithm SHA256).Hash.ToLowerInvariant();columns=@($rows[0].psobject.Properties.Name)}
    Write-Host ('Captured '+$mode+' frame records. No cadence verdict has been calculated.')
}
if(Test-Path -LiteralPath $log){Copy-Item -LiteralPath $log -Destination (Join-Path $run 'skyrim-after.log')}
[ordered]@{schema=1;presentMonVersion='2.6.0';presentMonSha256=$expectedHash;captures=$captures;analysis='Pending: identify the native display swapchain, exclude overlay/focus transitions, inspect display metrics and dropped frames; CSV row count alone is not scanout evidence';settingsEdited=$false}|ConvertTo-Json -Depth 8|Set-Content -LiteralPath (Join-Path $run 'capture.json') -Encoding utf8
Write-Host ('Capture complete: '+$run)
Write-Host 'FG should now be on. Close Skyrim normally and tell Codex capture done, plus any image/window problem.'
