param([Parameter(Mandatory)][string]$CaptureDirectory)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($CaptureDirectory)
$receipt=Get-Content -LiteralPath (Join-Path $root 'capture.json') -Raw|ConvertFrom-Json
function Read-Time([string]$text){
    $normalized=[regex]::Replace($text,'(\.\d{7})\d{2}$','${1}')
    [datetime]::ParseExact($normalized,'yyyy-M-d H:m:s.fffffff',[Globalization.CultureInfo]::InvariantCulture)
}
function Number([string]$text){if($text -eq 'NA' -or -not $text){return $null};[double]::Parse($text,[Globalization.CultureInfo]::InvariantCulture)}
function Summary($values){
    $sorted=@($values|Where-Object {$null -ne $_}|Sort-Object)
    if(-not $sorted.Count){return $null}
    $result=[ordered]@{count=$sorted.Count;mean=($sorted|Measure-Object -Average).Average;min=$sorted[0];max=$sorted[-1]}
    foreach($percent in @(50,95,99)){
        $index=($sorted.Count-1)*$percent/100
        $lo=[int][math]::Floor($index);$hi=[int][math]::Ceiling($index)
        $result['p'+$percent]=$sorted[$lo]+($sorted[$hi]-$sorted[$lo])*($index-$lo)
    }
    $result
}
$phases=@()
foreach($capture in $receipt.captures){
    $path=Join-Path $root $capture.csv
    if((Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToLowerInvariant() -ne $capture.sha256){throw 'CSV hash mismatch'}
    $rows=@(Import-Csv -LiteralPath $path)
    if($rows.Count -ne $capture.rows){throw 'CSV row count differs from receipt'}
    if(@($rows|Group-Object SwapChainAddress).Count -ne 1){throw 'Multiple swapchains need separate analysis'}
    if(@($rows|Where-Object {$_.ProcessID -ne [string]$capture.processId -or $_.Application -ne 'SkyrimSE.exe' -or $_.PresentMode -ne 'Hardware: Independent Flip'}).Count){throw 'Unexpected process/present mode'}
    $first=Read-Time $rows[0].TimeInDateTime;$last=Read-Time $rows[-1].TimeInDateTime
    $trimmed=@($rows|Where-Object {(Read-Time $_.TimeInDateTime) -ge $first.AddSeconds(2) -and (Read-Time $_.TimeInDateTime) -lt $last.AddSeconds(-2)})
    $display=Summary @($trimmed|ForEach-Object {Number $_.MsBetweenDisplayChange}|Where-Object {$null -ne $_ -and $_ -gt 0})
    $api=Summary @($trimmed|ForEach-Object {Number $_.MsBetweenPresents})
    $latency=Summary @($trimmed|ForEach-Object {Number $_.MsUntilDisplayed})
    $started=if($capture.startedUtc -is [datetime]){[datetimeoffset]$capture.startedUtc}else{[datetimeoffset]::Parse($capture.startedUtc,[Globalization.CultureInfo]::InvariantCulture)}
    $phaseStartUtc=$started.ToUniversalTime().AddSeconds($capture.delaySeconds)
    $phases+=[ordered]@{mode=$capture.requestedMode;swapchain=$rows[0].SwapChainAddress;processId=$capture.processId;presentMode=$rows[0].PresentMode;tearingAllowed=$rows[0].AllowsTearing;totalRows=$rows.Count;selectedRows=$trimmed.Count;recordedSpanSeconds=($last-$first).TotalSeconds;firstRawCsvTime=$rows[0].TimeInDateTime;lastRawCsvTime=$rows[-1].TimeInDateTime;collectorStartUtc=$phaseStartUtc.ToString('o');collectorEndUtc=$phaseStartUtc.AddSeconds(30).ToString('o');positiveDisplayIntervals=$display;apiIntervals=$api;displayLatency=$latency;displayUpdateRateHz=1000/$display.mean;nonPositiveOrUnavailableDisplayRows=@($trimmed|Where-Object {$null -eq (Number $_.MsBetweenDisplayChange) -or (Number $_.MsBetweenDisplayChange) -le 0}).Count;unavailableDisplayLatencyRows=@($trimmed|Where-Object {$null -eq (Number $_.MsUntilDisplayed)}).Count}
}
if($phases[0].swapchain -ne $phases[1].swapchain){throw 'Swapchain changed between phases'}
[ordered]@{captureDirectory=$root;analysis='Trim2s from each end; mean positive MsBetweenDisplayChange determines ETW display-update rate; percentiles use linear interpolation';timestampCaveat='CSV wall-clock fields are three hours ahead of collector UTC converted to Moscow and Skyrim log. Phase alignment uses collector startedUtc plus5s delay, not raw CSV clock; relative CSV durations retained.';phases=$phases;displayRateRatioOnOff=$phases[1].displayUpdateRateHz/$phases[0].displayUpdateRateHz;limitations=@('Allows tearing: update rate is not proof that every frame was displayed in full','No FrameType markers; individual generated frames are not labelled','No optical scanout or input-to-photon latency measurement','No claim that absence of NA records proves absence of all dropped frames')}|ConvertTo-Json -Depth 12
