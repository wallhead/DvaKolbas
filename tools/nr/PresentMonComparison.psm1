function Convert-PmTime([string]$Text){
    $normalized=[regex]::Replace($Text,'(\.\d{7})\d{2}$','${1}')
    [datetime]::ParseExact($normalized,'yyyy-M-d H:m:s.fffffff',[Globalization.CultureInfo]::InvariantCulture)
}
function Convert-PmNumber([string]$Text){
    if([string]::IsNullOrWhiteSpace($Text) -or $Text -eq 'NA'){return $null}
    $value=0.0
    if(-not [double]::TryParse($Text,[Globalization.NumberStyles]::Float,[Globalization.CultureInfo]::InvariantCulture,[ref]$value) -or
       [double]::IsNaN($value) -or [double]::IsInfinity($value) -or $value -lt 0){throw "Invalid PresentMon number: $Text"}
    $value
}
function Get-PmDistribution($Values){
    $sorted=@($Values|Where-Object {$null -ne $_}|Sort-Object)
    if(-not $sorted.Count){return [pscustomobject]@{count=0;mean=$null;median=$null;p95=$null;p99=$null}}
    $result=[ordered]@{count=$sorted.Count;mean=($sorted|Measure-Object -Average).Average}
    foreach($entry in @(@('median',.5),@('p95',.95),@('p99',.99))){
        $index=($sorted.Count-1)*$entry[1];$lo=[int][math]::Floor($index);$hi=[int][math]::Ceiling($index)
        $result[$entry[0]]=$sorted[$lo]+($sorted[$hi]-$sorted[$lo])*($index-$lo)
    }
    [pscustomobject]$result
}
function Get-NrPresentMonSummary {
    param([object[]]$Rows,[int]$ProcessId,[int]$CaptureSeconds=0)
    if(-not $Rows.Count){throw 'PresentMon CSV contains no rows'}
    foreach($column in @('Application','ProcessID','SwapChainAddress','TimeInDateTime','MsBetweenPresents','MsBetweenDisplayChange','MsUntilDisplayed')){
        if($null -eq $Rows[0].psobject.Properties[$column]){throw "Missing PresentMon column: $column"}
    }
    if(@($Rows|Where-Object {$_.Application -ne 'SkyrimSE.exe' -or $_.ProcessID -ne [string]$ProcessId -or -not $_.SwapChainAddress}).Count){throw 'Unexpected process identity or swapchain'}
    $streams=@(foreach($group in $Rows|Group-Object SwapChainAddress){
        $timed=@($group.Group|ForEach-Object {[pscustomobject]@{time=Convert-PmTime $_.TimeInDateTime;row=$_}}|Sort-Object time)
        $first=$timed[0].time;$last=$timed[-1].time
        $selected=@($timed|Where-Object {$_.time -ge $first.AddSeconds(2) -and $_.time -le $last.AddSeconds(-2)})
        $app=@($selected|ForEach-Object {Convert-PmNumber $_.row.MsBetweenPresents}|Where-Object {$null -ne $_ -and $_ -gt 0})
        $display=@($selected|ForEach-Object {Convert-PmNumber $_.row.MsBetweenDisplayChange}|Where-Object {$null -ne $_ -and $_ -gt 0})
        $gpu=@($selected|ForEach-Object {Convert-PmNumber $_.row.MsGPUTime}|Where-Object {$null -ne $_})
        $appStats=Get-PmDistribution $app;$displayStats=Get-PmDistribution $display
        [pscustomobject]@{swapchain=$group.Name;totalRows=$group.Count;selectedRows=$selected.Count
            rawSpanSeconds=($last-$first).TotalSeconds;presentModes=@($group.Group.PresentMode|Sort-Object -Unique)
            appIntervals=$appStats;displayIntervals=$displayStats;gpuIntervals=Get-PmDistribution $gpu
            appPresentFps=$(if($appStats.count){1000/$appStats.mean}else{$null})
            displayUpdateRateHz=$(if($displayStats.count){1000/$displayStats.mean}else{$null})
            invalidAppIntervals=$selected.Count-$appStats.count
            notDisplayedOrUnknownRows=@($selected|Where-Object {$null -eq (Convert-PmNumber $_.row.MsUntilDisplayed)}).Count
            timingComplete=($appStats.count -ge 2 -and $appStats.count -eq $selected.Count)
            qualifiedTiming=($appStats.count -ge 2 -and $appStats.count -eq $selected.Count -and ($last-$first).TotalSeconds -ge $CaptureSeconds-5)
        }
    })
    [pscustomobject]@{streams=$streams;trimSecondsEachEnd=2
        metricScope='App Present intervals; source-frame FPS only if FG and independent FrameWarp are actually disabled. Display-update rate is separate.'
        swapchainSelection='Every swapchain reported separately; none selected automatically.'
        limitations=@('Host and NR state labels are user supplied, not observed feature state','No generated-frame classification or optical scanout measurement','Unavailable display timings do not alone prove dropped frames','Timestamp subtraction uses relative CSV time; no UTC/log alignment is claimed')}
}
Export-ModuleMember -Function Get-NrPresentMonSummary
