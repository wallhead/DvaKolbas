param(
    [Parameter(Mandatory)][string]$SourceIni,
    [Parameter(Mandatory)][string]$OutputIni
)
$ErrorActionPreference='Stop'
# Explicit offline conversion only. The game accepts the named layout exclusively.
. (Join-Path $PSScriptRoot '../fsr/PackageCommon.ps1')
$source=(Resolve-Path -LiteralPath $SourceIni).Path
$output=[IO.Path]::GetFullPath($OutputIni)
if($source -eq $output -or (Test-Path -LiteralPath $output)){throw 'Use a new output file; source INI stays untouched'}
$sourceHash=(Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash
$lines=Set-PackageIniValues ([IO.File]::ReadAllLines($source)) @{}
$temporary=$output+'.tmp-'+[Guid]::NewGuid().ToString('N')
try {
    [IO.File]::WriteAllLines($temporary,$lines,[Text.UTF8Encoding]::new($false))
    $before=Read-PackageIni $source;$after=Read-PackageIni $temporary
    foreach($key in $before.Keys){
        if($key -in @('Settings/ConfigVersion','/ConfigVersion','SourceDLSSG/NRStableColors','NeuralRendering/StableColors')){continue}
        # DLAA's dormant numeric quality and disabled sharpening strength are not
        # effective controls: Native and zero encode those states explicitly.
        if($before['Settings/UpscaleType'] -eq '3' -and $key -in @('DLSS/QualityLevel','Settings/QualityLevel')){continue}
        if($before['Settings/Sharpening'] -eq 'false' -and $key -eq 'Settings/Sharpness'){continue}
        if($before['Settings/UseOptimalMipLodBias'] -eq 'true' -and $key -eq 'Settings/MipLodBias'){continue}
        if($key -eq 'Hotkeys/ToggleOverlay' -and $before[$key] -match '^0[xX]([0-9a-fA-F]+)$'){
            if([string][Convert]::ToInt32($Matches[1],16) -ceq $after[$key]){continue}
        }
        if(-not $after.ContainsKey($key) -or $after[$key] -cne $before[$key]){throw "INI value changed: $key"}
    }
    if((Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash -ne $sourceHash){throw 'Source INI changed during conversion'}
    Move-Item -LiteralPath $temporary -Destination $output
} finally {
    if(Test-Path -LiteralPath $temporary){Remove-Item -LiteralPath $temporary}
}
Write-Output "CONVERTED: $output (source preserved)"
