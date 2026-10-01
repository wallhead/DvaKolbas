$ErrorActionPreference = 'Stop'
$workspace = Split-Path $PSScriptRoot -Parent
$results = @()
foreach ($edition in 'standard','universal') {
    $destination = Join-Path $workspace "out/packages/TRP-0.3.5-$edition-246d152c-dlss310.8-streamline2.13"
    $guide = if ($edition -eq 'standard') { 'package/STANDARD-README.md' } else { 'package/README.md' }
    Copy-Item -LiteralPath (Join-Path $workspace $guide) -Destination (Join-Path $destination 'UPSTREAM-README.md')
    @"
# TRP 0.3.5 $edition local baseline

Built from revision 246d152c42a83308beb7a6f4930e7c472ef0d4ce without renderer changes.
Includes the user-supplied DLSS/DLSS-G 310.8.0.0, Streamline 2.13.0.0 and NR 310.8.0.0.
No additional runtime download is needed. Streamline identifies itself in its log
as v2.13.0-beta10 (11ffc00ec).

Game acceptance has NOT RUN. An independent synthetic FG probe failed with this
runtime bundle on its second Present (DXGI_ERROR_INVALID_CALL). A separate 2.14.1 /
310.9.1 bundle passed eight synthetic frames. Neither result establishes TRP gameplay
behavior. This package needs its own game test.

Use a separate MO2 test profile; disable competing renderer/upscaler/FG mods.
Enable HAGS and use windowed or borderless mode. Launch SKSE, open TRP with End,
verify DLSS with FG off, then test native x2 on RTX 40 hardware. Keep NR and HDR off
for this first check. Collect TheosRenderPipeline.log and skse64.log afterwards.

[Upstream guide](UPSTREAM-README.md) describes controls and compatibility. Its runtime
downloads, package identities and historical tests refer to upstream packages;
the versions above and build-manifest.json describe this local package.

LICENSE and THIRD-PARTY.md contain renderer and incorporated-code notices.
NVIDIA-Streamline-license.txt is from the SDK 2.11.1 used for compilation, and does
not replace the terms accompanying the user-supplied runtime archive.
"@ | Set-Content -LiteralPath (Join-Path $destination 'README.md') -Encoding utf8
    $manifestPath = Join-Path $destination 'build-manifest.json'
    $manifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
    $manifest.files = @(Get-ChildItem -LiteralPath $destination -Recurse -File | Where-Object { $_.FullName -ne $manifestPath } | ForEach-Object {
        [ordered]@{ path=[IO.Path]::GetRelativePath($destination,$_.FullName).Replace('\','/'); bytes=$_.Length; sha256=(Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant() }
    })
    $manifest | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath $manifestPath -Encoding utf8
    Compress-Archive -Path (Join-Path $destination '*') -DestinationPath "$destination.zip" -Force
    foreach ($file in $manifest.files) {
        $actual = Get-FileHash -LiteralPath (Join-Path $destination $file.path) -Algorithm SHA256
        if ($actual.Hash.ToLowerInvariant() -ne $file.sha256) { throw "Hash mismatch: $($file.path)" }
    }
    $zip = [IO.Compression.ZipFile]::OpenRead("$destination.zip")
    try {
        foreach ($file in $manifest.files) {
            $entry = $zip.GetEntry($file.path)
            if (-not $entry) { throw "Missing ZIP entry: $($file.path)" }
            $stream = $entry.Open()
            try {
                $digest = [Convert]::ToHexString([Security.Cryptography.SHA256]::HashData($stream)).ToLowerInvariant()
                if ($digest -ne $file.sha256) { throw "ZIP hash mismatch: $($file.path)" }
            } finally { $stream.Dispose() }
        }
    } finally { $zip.Dispose() }
    $results += [ordered]@{ edition=$edition; zip="$destination.zip"; sha256=(Get-FileHash -LiteralPath "$destination.zip" -Algorithm SHA256).Hash.ToLowerInvariant(); verifiedManifestFiles=$manifest.files.Count; result='PASS: staged files and all ZIP entry hashes' }
}
$results | ConvertTo-Json -Depth 4 | Tee-Object -FilePath (Join-Path $workspace 'out/validation/package-verification.json')
