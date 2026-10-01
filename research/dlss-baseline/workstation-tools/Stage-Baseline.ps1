param(
    [ValidateSet('standard','universal')][string]$Edition = 'standard',
    [string]$RuntimeDirectory = 'out/runtime-dlss310.8-streamline2.13',
    [string]$PackageSuffix = 'dlss310.8-streamline2.13'
)
$ErrorActionPreference = 'Stop'
$workspace = Split-Path $PSScriptRoot -Parent
Push-Location $workspace
try {
    $binary = "out/build/$Edition/Release/TheosRenderPipeline.dll"
    if (-not (Test-Path -LiteralPath $binary -PathType Leaf)) { throw "Missing build output: $binary" }
    $name = "TRP-0.3.5-$Edition-246d152c-$PackageSuffix"
    $destination = Join-Path $workspace "out/packages/$name"
    if (Test-Path -LiteralPath $destination) { throw "Package already exists: $destination" }
    New-Item -ItemType Directory -Path $destination -Force | Out-Null
    Copy-Item -LiteralPath 'package/SKSE' -Destination $destination -Recurse
    Copy-Item -LiteralPath $binary -Destination (Join-Path $destination 'SKSE/Plugins/TheosRenderPipeline.dll')
    $renderer = Join-Path $destination 'SKSE/Plugins/TheosRenderPipeline'
    $streamline = Join-Path $renderer 'NVIDIA/Streamline'
    New-Item -ItemType Directory -Path $streamline -Force | Out-Null
    foreach ($name in 'sl.interposer.dll','sl.common.dll','sl.dlss_g.dll','sl.reflex.dll','sl.pcl.dll','nvngx_dlssg.dll') {
        Copy-Item -LiteralPath (Join-Path $RuntimeDirectory $name) -Destination $streamline
    }
    Copy-Item -LiteralPath (Join-Path $RuntimeDirectory 'nvngx_dlss.dll') -Destination $renderer
    $nr = Join-Path $RuntimeDirectory 'nvngx_dlssnr.dll'
    if (Test-Path -LiteralPath $nr -PathType Leaf) { Copy-Item -LiteralPath $nr -Destination (Join-Path $renderer 'NVIDIA') }
    Copy-Item -LiteralPath 'LICENSE','THIRD-PARTY.md' -Destination $destination
    $guide = if ($Edition -eq 'standard') { 'package/STANDARD-README.md' } else { 'package/README.md' }
    Copy-Item -LiteralPath $guide -Destination (Join-Path $destination 'UPSTREAM-README.md')
    $srVersion = (Get-Item -LiteralPath (Join-Path $RuntimeDirectory 'nvngx_dlss.dll')).VersionInfo.FileVersion
    $fgVersion = (Get-Item -LiteralPath (Join-Path $RuntimeDirectory 'nvngx_dlssg.dll')).VersionInfo.FileVersion
    $slVersion = (Get-Item -LiteralPath (Join-Path $RuntimeDirectory 'sl.interposer.dll')).VersionInfo.FileVersion
    @"
# TRP 0.3.5 $Edition local baseline

Built from source revision 246d152c42a83308beb7a6f4930e7c472ef0d4ce without renderer changes.
This package includes the user-supplied NVIDIA runtimes: DLSS $srVersion,
DLSS-G $fgVersion and Streamline $slVersion. The NR runtime is included too.
No additional runtime download is needed for this package.

Game acceptance has NOT RUN. Building and passing offline checks do not establish
that DLSS frame generation works in Skyrim with this bundle.

Install through MO2 in a separate test profile and disable competing renderer,
upscaler and frame-generation mods. Enable HAGS and use windowed or borderless mode.
Launch SKSE, open TRP with End, and verify DLSS with FG off before testing native x2
on an RTX 40-series GPU. Keep NR and HDR off during the initial FG baseline check.

[Upstream guide](UPSTREAM-README.md) describes the renderer controls and compatibility.
Its runtime downloads, release-package identities and historical test reports refer
to upstream packages and are not evidence for this local build. The runtime versions
listed above and build-manifest.json describe this package.

LICENSE and THIRD-PARTY.md contain renderer and incorporated-code notices.
NVIDIA-Streamline-license.txt accompanies the SDK 2.11.1 headers used for compilation;
it is not a substitute for the terms accompanying the user-supplied runtime archive.
"@ | Set-Content -LiteralPath (Join-Path $destination 'README.md') -Encoding utf8
    Copy-Item -LiteralPath '.dependencies/streamline-sdk-v2.11.1/license.txt' -Destination (Join-Path $destination 'NVIDIA-Streamline-license.txt')
    $files = Get-ChildItem -LiteralPath $destination -Recurse -File | ForEach-Object {
        [ordered]@{ path=[IO.Path]::GetRelativePath($destination,$_.FullName).Replace('\','/'); bytes=$_.Length; sha256=(Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant() }
    }
    [ordered]@{ edition=$Edition; revision='246d152c42a83308beb7a6f4930e7c472ef0d4ce'; runtimes=[IO.Path]::GetFullPath($RuntimeDirectory); gameAcceptance='NOT RUN'; files=@($files) } | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $destination 'build-manifest.json') -Encoding utf8
    Compress-Archive -Path (Join-Path $destination '*') -DestinationPath "$destination.zip"
    Get-Item -LiteralPath "$destination.zip" | Select-Object FullName,Length
    Get-FileHash -LiteralPath "$destination.zip" -Algorithm SHA256
} finally { Pop-Location }
