param(
    [ValidateSet('rtx40','aio19-rtx40','aio19-rtx40-shim')][string]$RuntimeProfile='rtx40',
    [string]$RuntimeDll,
    [string]$DriverCore='C:/Windows/System32/DriverStore/FileRepository/nv_dispi.inf_amd64_da865124972e1f80/_nvngx.dll',
    [ValidateRange(2,1000)][int]$Frames=300,
    [ValidateRange(0,240)][int]$Warmup=120,
    [ValidateRange(16,3840)][int]$Width=2560,
    [ValidateRange(16,2160)][int]$Height=1440,
    [ValidateSet('on','off')][string]$Enabled='on',
    [ValidateSet('on','off')][string]$Instrumentation='on',
    [ValidateSet('on','off')][string]$Readback='off',
    [ValidateRange(0,1)][int]$TimerPeriodMs=1,
    [ValidateRange(1,3)][int]$Repeats=3,
    [string]$Executable,
    [string]$FsrRuntime,
    [ValidateSet('on','off')][string]$PreparedFsr='off',
    [string]$Output
)
$ErrorActionPreference='Stop'
$taskRoot=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
if(-not $Executable){$Executable=Join-Path $taskRoot 'out/build/nr-runtime/Release/TRPNrPerformanceProbe.exe'}
if(-not $RuntimeDll){
    if($RuntimeProfile -eq 'rtx40'){$RuntimeDll='C:/Users/user/Downloads/nvngx_dlssnr_4_series/nvngx_dlssnr.dll'}
    else{$RuntimeDll=Join-Path $taskRoot 'out/research/aio19/extracted/UpscalerBasePlugin/nvngx_dlssnr.dll'}
}
if(-not $Output){$Output=Join-Path $taskRoot "out/research/nr/performance/$RuntimeProfile-$Enabled-$Instrumentation-$Readback-timer$TimerPeriodMs"}
foreach($file in @($Executable,$RuntimeDll,$DriverCore)){if(-not (Test-Path -LiteralPath $file -PathType Leaf)){throw "Required file missing: $file"}}
for($run=1;$run -le $Repeats;$run++){
    if(Get-Process SkyrimSE -ErrorAction SilentlyContinue){throw 'Close Skyrim before standalone GPU work.'}
    $receiptPath="$Output-$run.json"
    $probeArgs=@('--profile',$RuntimeProfile,'--dll',$RuntimeDll,'--core',$DriverCore,'--frames',"$Frames",'--warmup',"$Warmup",'--width',"$Width",'--height',"$Height",'--enabled',$Enabled,'--instrumentation',$Instrumentation,'--readback',$Readback,'--timer-period-ms',"$TimerPeriodMs",'--output',$receiptPath)
    if($FsrRuntime){$probeArgs+=@('--fsr-runtime',[IO.Path]::GetFullPath($FsrRuntime))}
    $probeArgs+=@('--prepared-fsr',$PreparedFsr)
    & $Executable @probeArgs
    $probeExit=$LASTEXITCODE
    if($probeExit -ne 0){throw "Probe exited $probeExit; inspect $receiptPath"}
    $receipt=Get-Content -LiteralPath $receiptPath -Raw | ConvertFrom-Json
    if(-not $receipt.retired -or -not $receipt.timingComplete -or $receipt.samples.Count -ne $Frames){throw "Incomplete probe receipt: $receiptPath"}
    Write-Output "PASS run $run/$Repeats; samples=$Frames; profile=$RuntimeProfile; receipt=$receiptPath"
}
