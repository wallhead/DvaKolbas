[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildDirectory,[Parameter(Mandatory)][string]$FsrRuntimeDirectory,
    [Parameter(Mandatory)][string]$SourceIni,[Parameter(Mandatory)][string]$OutputDirectory,
    [Parameter(Mandatory)][string]$IntelSdkDirectory,[Parameter(Mandatory)][string]$ValidationReceipt,
    [Parameter(Mandatory)][string]$ProfileName,[string]$NeuralRuntimeDirectory='')
if([string]::IsNullOrWhiteSpace($ProfileName)){throw 'An explicit destination profile is required for this trial'}
& (Join-Path $PSScriptRoot 'Stage-Trial.ps1') -BuildDirectory $BuildDirectory -FsrRuntimeDirectory $FsrRuntimeDirectory -SourceIni $SourceIni -OutputDirectory $OutputDirectory -IntelSdkDirectory $IntelSdkDirectory -NeuralRuntimeDirectory $NeuralRuntimeDirectory -ValidationReceipt $ValidationReceipt -XessFrameGeneration
