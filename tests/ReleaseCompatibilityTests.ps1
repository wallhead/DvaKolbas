param(
    [Parameter(Mandatory)][string]$TemplateArchive,
    [Parameter(Mandatory)][string]$StandardDll
)
$ErrorActionPreference='Stop'
$root=Join-Path ([IO.Path]::GetTempPath()) ('raz-release-compat-'+[guid]::NewGuid())
[IO.Directory]::CreateDirectory($root)|Out-Null
$archive=Join-Path $root 'RaZKolbaS DLSS FSR FG NR v1.3.3.zip'
$stage=Join-Path $root 'stage'
try {
    $rejected=$false
    try {
        & (Join-Path $PSScriptRoot '../tools/nr/Pack-Release.ps1') -TemplateArchive $TemplateArchive `
            -PluginDll $StandardDll -OutputArchive $archive -StagingDirectory $stage
    } catch {
        if($_.Exception.Message -notmatch 'Universal.*RTX 20/30'){throw}
        $rejected=$true
    }
    if(-not $rejected){throw 'All-GPU release incorrectly accepted the Standard renderer that fails on RTX 3050'}
    if((Test-Path -LiteralPath $archive) -or (Test-Path -LiteralPath $stage)){
        throw 'Rejected Standard release must not create an archive or staging directory'
    }
    Write-Output 'PASS: all-GPU release rejects Standard before writing output; RTX 20/30 compatibility is required'
} finally {
    $full=[IO.Path]::GetFullPath($root)
    $temp=[IO.Path]::GetFullPath([IO.Path]::GetTempPath())
    if(-not $full.StartsWith($temp,[StringComparison]::OrdinalIgnoreCase) -or
        [IO.Path]::GetFileName($full) -notlike 'raz-release-compat-*'){throw 'Unsafe scratch cleanup'}
    Remove-Item -LiteralPath $full -Recurse -Force
}
