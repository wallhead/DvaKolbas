param(
    [ValidateSet('rtx40','rtx50','rtx20-30')][string]$Profile='rtx40',
    [string]$RuntimeDll='C:/Users/user/Downloads/nvngx_dlssnr_4_series/nvngx_dlssnr.dll',
    [string]$DriverCore='C:/Windows/System32/DriverStore/FileRepository/nv_dispi.inf_amd64_da865124972e1f80/_nvngx.dll',
    [ValidateSet('on','off')][string]$CallerShim='on',
    [ValidateRange(2,240)][int]$Frames=30,
    [string]$Executable,
    [string]$Output
)
$ErrorActionPreference='Stop'
$taskRoot=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
if(-not $Executable){$Executable=Join-Path $taskRoot 'out/build/nr-runtime/Release/TRPNrRuntimeProbe.exe'}
if(-not $Output){$Output=Join-Path $taskRoot "out/research/nr/runtime/$Profile-$CallerShim.json"}
if(Get-Process SkyrimSE -ErrorAction SilentlyContinue){throw 'Close Skyrim before running the standalone GPU probe.'}
foreach($file in @($Executable,$RuntimeDll,$DriverCore)){
    if(-not (Test-Path -LiteralPath $file -PathType Leaf)){throw "Required local file missing: $file"}
}
& $Executable --profile $Profile --dll $RuntimeDll --core $DriverCore --caller-shim $CallerShim --frames $Frames --output $Output
$probeExit=$LASTEXITCODE
if(-not (Test-Path -LiteralPath $Output -PathType Leaf)){throw "Probe exited $probeExit without a receipt."}
$receipt=Get-Content -LiteralPath $Output -Raw | ConvertFrom-Json
$receipt | Select-Object result,profile,rawInit,rawShutdown,readbackFrames,distinctOutputHashes,spatiallyVariedFrames,failure | Format-List
exit $probeExit
