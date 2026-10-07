param([string]$KitDirectory=$PSScriptRoot)
$ErrorActionPreference='Stop'
$kit=[IO.Path]::GetFullPath($KitDirectory)
if(Get-Process SkyrimSE,SkyrimSELauncher -ErrorAction SilentlyContinue){throw 'Close Skyrim before running the standalone GPU test. MO2 can stay open.'}
$logs=Join-Path $kit 'results'
[IO.Directory]::CreateDirectory($logs) | Out-Null
function Invoke-Validation([string]$Executable,[string[]]$Arguments,[string]$Name) {
    $stdout=Join-Path $logs ($Name+'.log');$stderr=Join-Path $logs ($Name+'.stderr.log')
    $child=Start-Process -FilePath (Join-Path $kit $Executable) -ArgumentList $Arguments -WorkingDirectory $kit -WindowStyle Hidden -PassThru -RedirectStandardOutput $stdout -RedirectStandardError $stderr
    [void]$child.Handle
    $child.WaitForExit()
    $child.Refresh();$code=$child.ExitCode;$child.Dispose()
    Get-Content -LiteralPath $stdout
    if((Get-Item -LiteralPath $stderr).Length){Get-Content -LiteralPath $stderr}
    if($null -eq $code -or $code -ne 0){throw "$Name failed (exit $code). Send the results folder; do not install the trial yet."}
}
Write-Output 'Checking actual FSR4 FG provider and context. This does not modify Skyrim or MO2.'
Invoke-Validation 'TRPFsrGenerationProviderProbe.exe' @(('"'+(Join-Path $kit 'plugins')+'"'),'--provider','MachineLearning','--create-presenter') 'ml-provider'
Write-Output 'Testing real/generated pixels, UI, off/on, menu suppression and context retirement.'
Invoke-Validation 'TRPFsrGenerationGpuSmoke.exe' @('--runtime',('"'+(Join-Path $kit 'plugins/FSR')+'"'),'--fg-provider','MachineLearning','--frames','360','--recreate','5','--debug','auto','--source-directory',('"'+(Join-Path $kit 'sources')+'"'),'--output',('"'+(Join-Path $logs 'ml-fg.json')+'"')) 'ml-gpu'
$receipt=Get-Content -LiteralPath (Join-Path $logs 'ml-fg.json') -Raw | ConvertFrom-Json
if($receipt.result -ne 'PASS' -or $receipt.fgActualProvider -ne '4.0.1' -or $receipt.generatedPixelReadbacks -le 0){throw 'Actual ML FG generated pixels were not qualified; send the results folder.'}
Write-Output 'PASS: actual FSR4 FG standalone checks. Send the results folder before the Skyrim trial; visible cadence and Skyrim acceptance remain pending.'
