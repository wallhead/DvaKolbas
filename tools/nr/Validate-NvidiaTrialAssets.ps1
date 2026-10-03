param([Parameter(Mandatory)][string]$PackageDirectory)
# Additional preflight for an NR trial preserving DLSS/DLAA user selectors.
# FSR-only qualification does not establish that these NVIDIA assets exist.
. (Join-Path $PSScriptRoot '../fsr/PackageCommon.ps1')
$root=[IO.Path]::GetFullPath($PackageDirectory)
$plugins=Join-Path $root 'SKSE/Plugins'
$ini=Read-PackageIni (Join-Path $plugins 'TheosRenderPipeline.ini')
if($ini['Settings/UpscaleType'] -notin @('0','3')) {throw 'This preflight requires a DLSS/DLAA-selected NR trial'}
$manifest=Get-Content -LiteralPath (Join-Path $root 'nr-trial-manifest.json') -Raw | ConvertFrom-Json
$configured=$ini['Experimental/SourceDLSSGStreamlineDirectory']
if([string]::IsNullOrWhiteSpace($configured)) {throw 'NVIDIA trial has no configured Streamline directory'}
$streamline=[IO.Path]::GetFullPath($(if([IO.Path]::IsPathRooted($configured)) {$configured} else {Join-Path $plugins $configured}))
$paths=@((Join-Path $plugins 'TheosRenderPipeline/nvngx_dlss.dll'),(Join-Path $root 'NVIDIA-Streamline-license.txt'))
foreach($name in @('nvngx_dlssg.dll','sl.common.dll','sl.dlss_g.dll','sl.interposer.dll','sl.pcl.dll','sl.reflex.dll')) {$paths+=Join-Path $streamline $name}
foreach($path in $paths) {
    $full=[IO.Path]::GetFullPath($path)
    if(-not $full.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)) {throw 'Packaged NVIDIA runtime lies outside the trial directory'}
    $relative=[IO.Path]::GetRelativePath($root,$full).Replace('\','/')
    $entries=@($manifest.files | Where-Object path -eq $relative)
    if($entries.Count -ne 1) {throw "Required NVIDIA runtime absent or ambiguous in trial manifest: $relative"}
    Assert-PinnedFile $full $entries[0].sha256 $entries[0].bytes
}
Write-Output 'PASS: selected DLSS/DLAA trial contains manifested DLSS SR, six Streamline modules and license; gameplay loading remains a separate check'
