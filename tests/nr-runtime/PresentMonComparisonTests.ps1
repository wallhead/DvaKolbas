$ErrorActionPreference='Stop'
Import-Module (Join-Path $PSScriptRoot '../../tools/nr/PresentMonComparison.psm1') -Force
function Check($value,[string]$message){if(-not $value){throw $message};Write-Output "PASS $message"}
$rows=@(foreach($chain in @('0xA','0xB')){foreach($second in 0..12){[pscustomobject]@{
    Application='SkyrimSE.exe';ProcessID='42';SwapChainAddress=$chain;PresentMode='Hardware: Independent Flip'
    TimeInDateTime=('2026-10-3 13:00:{0:00}.123456789' -f $second)
    MsBetweenPresents=$(if($chain -eq '0xA'){'20.0'}else{'40.0'})
    MsBetweenDisplayChange='20.0';MsGPUTime='NA';MsUntilDisplayed='NA'
}}})
$result=Get-NrPresentMonSummary -Rows $rows -ProcessId 42
Check ($result.streams.Count -eq 2) 'DistinctSwapchainsNeverMergedOrAutomaticallyChosen'
$first=$result.streams|Where-Object swapchain -eq '0xA'
Check ($first.selectedRows -eq 9) 'TrimTwoSecondsFromBothEndsUsingRelativeTime'
Check ([math]::Abs($first.appPresentFps-50) -lt .000001) 'AppRateUsesMeanPresentInterval'
Check ($first.appIntervals.p95 -eq 20 -and $first.appIntervals.p99 -eq 20) 'TailFrameTimeReportedInMilliseconds'
Check ($first.gpuIntervals.count -eq 0 -and $null -eq $first.gpuIntervals.mean) 'UnavailableGpuTimingsStayUnavailable'
Check ($first.notDisplayedOrUnknownRows -eq 9) 'UnavailableDisplayRowsRetainedWithoutClaimingDroppedFrames'
$partial=@($rows|Where-Object {$_.SwapChainAddress -eq '0xA' -or $_.TimeInDateTime -le '2026-10-3 13:00:03.123456789'})
$mixed=Get-NrPresentMonSummary -Rows $partial -ProcessId 42 -CaptureSeconds 12
Check (@($mixed.streams|Where-Object qualifiedTiming).Count -eq 1) 'ShortSecondarySwapchainDoesNotInvalidateFullPrimaryCapture'
$rows[4].MsBetweenPresents='NA'
$first=(Get-NrPresentMonSummary -Rows $rows -ProcessId 42).streams|Where-Object swapchain -eq '0xA'
Check ($first.invalidAppIntervals -eq 1 -and -not $first.timingComplete) 'MissingAppTimingInvalidatesCompleteCapture'
$rows[4].MsBetweenPresents='20.0';$rows[0].ProcessID='99'
try{$null=Get-NrPresentMonSummary -Rows $rows -ProcessId 42;throw 'Foreign process was accepted'}catch{Check ($_.Exception.Message -match 'process identity') 'ForeignProcessRejected'}
