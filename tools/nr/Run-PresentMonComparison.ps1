param(
    [ValidateSet('AIO19','DvaKolbas')][string]$BenchmarkHost,
    [ValidateRange(10,180)][int]$Seconds=60,
    [ValidateRange(1,3)][int]$Repeats=1,
    [string]$PresentMonPath,
    [switch]$Preflight
)
$ErrorActionPreference='Stop'
$repository=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
Import-Module (Join-Path $PSScriptRoot 'PresentMonComparison.psm1') -Force
if(-not $PresentMonPath){
    $PresentMonPath=Join-Path $repository 'out/research/presentmon/PresentMon-2.6.0-x64.exe'
    if(-not (Test-Path -LiteralPath $PresentMonPath)){$PresentMonPath=Join-Path ([Environment]::GetFolderPath('UserProfile')) 'Downloads/PresentMon-2.6.0-x64.exe'}
}
$PresentMonPath=[IO.Path]::GetFullPath($PresentMonPath)
$expectedHash='b2a706bc6ad475749e3b7e3409263aa1e6906d45bdcf993f6dbc0f660188f1af'
if((Get-FileHash -LiteralPath $PresentMonPath -Algorithm SHA256).Hash.ToLowerInvariant() -ne $expectedHash){throw 'Expected the verified PresentMon 2.6.0 binary'}
$signature=Get-AuthenticodeSignature -LiteralPath $PresentMonPath
if($signature.Status -ne 'Valid' -or $signature.SignerCertificate.Subject -notmatch 'CN=Intel Corporation'){throw 'Expected valid Intel signature'}
$identity=[Security.Principal.WindowsIdentity]::GetCurrent()
$administrator=([Security.Principal.WindowsPrincipal]::new($identity)).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
$games=@(Get-Process SkyrimSE -ErrorAction SilentlyContinue)
if($Preflight){
    [ordered]@{binaryVerified=$true;version='2.6.0';administrator=$administrator;skyrimProcesses=$games.Count;ready=($administrator -and $games.Count -eq 1);settingsEdited=$false}|ConvertTo-Json
    exit 0
}
if(-not $administrator){throw 'Right-click Start-NR-PresentMon.cmd and choose Run as administrator. Windows ETW capture requires elevation on this machine.'}
if($games.Count -ne 1){throw 'Start Skyrim through the usual MO2 entry and load the test save first.'}
if(-not $BenchmarkHost){
    Write-Host 'Which upscaler is running? 1 = AIO19; 2 = DvaKolbas'
    switch(Read-Host 'Choose 1 or 2'){'1'{$BenchmarkHost='AIO19'}'2'{$BenchmarkHost='DvaKolbas'}default{throw 'Choose 1 or 2'}}
}
$game=$games[0]
$run=Join-Path $repository ('out/research/nr/presentmon/'+(Get-Date -Format 'yyyyMMdd-HHmmss')+'-'+$BenchmarkHost+'-'+[guid]::NewGuid().ToString('N'))
[IO.Directory]::CreateDirectory($run)|Out-Null
$receipt=[ordered]@{schema=1;benchmarkHost=$BenchmarkHost;processId=$game.Id;presentMonVersion='2.6.0';presentMonSha256=$expectedHash
    sceneNotes=Read-Host 'Brief scene/save label (use the same label for both upscalers)'
    settingsEdited=$false;settingsVerification='User confirmation only: FG/FrameWarp off, native input, one Before pass, tone0; verify actual INIs/logs separately'
    captures=@();status='in-progress';failure=''}
function Save-Receipt{$receipt|ConvertTo-Json -Depth 15|Set-Content -LiteralPath (Join-Path $run 'capture.json') -Encoding utf8}
Save-Receipt
try{
    Write-Host 'Keep FG and AIO FrameWarp OFF. Match native resolution, one NR Before pass, preset/style0, intensity/structure1, tone0, SR quality/provider and ENB/caps/VSync.'
    Write-Host 'For AIO ChainVersion1, set Tone0 in NR PASS1 too. Use the same save, view, time and weather. This script changes no settings.'
    foreach($repeat in 1..$Repeats){foreach($mode in @('Off','On')){
        $null=Read-Host ("Set NR $mode in Skyrim, close all overlays and warm up in the fixed scene for 30 seconds. Then press Enter here ($BenchmarkHost; run $repeat/$Repeats)")
        $game.Refresh();if($game.HasExited){throw 'Skyrim exited before capture'}
        $base='nr-'+$mode.ToLowerInvariant()+'-'+$repeat
        $csv=Join-Path $run ($base+'.csv');$stdout=Join-Path $run ($base+'-stdout.log');$stderr=Join-Path $run ($base+'-stderr.log')
        $arguments=@('--process_id',[string]$game.Id,'--session_name',('DvaKolbas-NR-'+[guid]::NewGuid().ToString('N')),
            '--delay','10','--timed',[string]$Seconds,'--terminate_after_timed','--terminate_on_proc_exit',
            '--no_console_stats','--no_track_input','--date_time','--write_display_metadata','--output_file',('"'+$csv+'"'))
        Write-Host "Return to Skyrim within 10 seconds. Keep the game foreground and the same view for $Seconds seconds."
        $started=[datetime]::UtcNow.ToString('o')
        $capture=Start-Process -FilePath $PresentMonPath -ArgumentList $arguments -WindowStyle Hidden -PassThru -Wait -RedirectStandardOutput $stdout -RedirectStandardError $stderr
        if($capture.ExitCode -ne 0){throw "PresentMon exited $($capture.ExitCode); inspect $stderr"}
        $game.Refresh();if($game.HasExited){throw 'Skyrim exited during capture; run incomplete'}
        $rows=@(Import-Csv -LiteralPath $csv)
        $summary=Get-NrPresentMonSummary -Rows $rows -ProcessId $game.Id -CaptureSeconds $Seconds
        if(-not @($summary.streams|Where-Object qualifiedTiming).Count){throw 'No full-duration swapchain timing; keep CSV for inspection and repeat the run'}
        $receipt.captures+=[ordered]@{nrState=$mode;repeat=$repeat;startedUtc=$started;delaySeconds=10;seconds=$Seconds;rows=$rows.Count
            csv=[IO.Path]::GetFileName($csv);sha256=(Get-FileHash -LiteralPath $csv -Algorithm SHA256).Hash.ToLowerInvariant();summary=$summary}
        Save-Receipt
        $summary.streams|Select-Object swapchain,qualifiedTiming,appPresentFps,@{Name='MedianMs';Expression={$_.appIntervals.median}},@{Name='P95Ms';Expression={$_.appIntervals.p95}},@{Name='P99Ms';Expression={$_.appIntervals.p99}}|Format-Table -AutoSize
    }}
    $receipt.status='complete';Save-Receipt
    Write-Host "Saved CSVs and summary: $run"
    Write-Host 'Next: close Skyrim normally, switch to the other upscaler yourself, and run this launcher again with the same scene/settings.'
}catch{$receipt.status='failed';$receipt.failure=$_.Exception.Message;Save-Receipt;throw}
