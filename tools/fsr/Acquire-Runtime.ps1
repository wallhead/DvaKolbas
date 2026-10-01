[CmdletBinding()]
param(
    [string]$Destination = (Join-Path $PSScriptRoot '../../.dependencies/FidelityFX-SDK-v2.3.0'),
    [string]$ArchivePath
)
$ErrorActionPreference = 'Stop'
$pin = Get-Content -LiteralPath (Join-Path $PSScriptRoot 'runtime-pin.json') -Raw | ConvertFrom-Json
$destinationRoot = [IO.Path]::GetFullPath($Destination)
[IO.Directory]::CreateDirectory($destinationRoot) | Out-Null
function Assert-Hash([string]$Path, [string]$Expected) {
    $actual = (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant()
    if ($actual -ne $Expected) { throw "SHA-256 mismatch: $Path" }
}
function Get-PinnedFile([string]$Url, [string]$Path, [string]$Hash) {
    if (Test-Path -LiteralPath $Path) { Assert-Hash $Path $Hash; return }
    [IO.Directory]::CreateDirectory([IO.Path]::GetDirectoryName($Path)) | Out-Null
    $temporary = "$Path.$([guid]::NewGuid().ToString('N')).part"
    Invoke-WebRequest -Uri $Url -OutFile $temporary -UseBasicParsing
    Assert-Hash $temporary $Hash
    Move-Item -LiteralPath $temporary -Destination $Path
}
if (-not $ArchivePath) {
    $ArchivePath = Join-Path $destinationRoot $pin.archive.filename
    Get-PinnedFile $pin.archive.url $ArchivePath $pin.archive.sha256
} else {
    $ArchivePath = [IO.Path]::GetFullPath($ArchivePath)
    Assert-Hash $ArchivePath $pin.archive.sha256
}
foreach ($header in $pin.headers) {
    $target = Join-Path $destinationRoot $header.path
    $url = "https://raw.githubusercontent.com/GPUOpen-LibrariesAndSDKs/FidelityFX-SDK/$($pin.commit)/$($header.path)"
    Get-PinnedFile $url $target $header.sha256
}
Add-Type -AssemblyName System.IO.Compression.FileSystem
$archive = [IO.Compression.ZipFile]::OpenRead($ArchivePath)
$receipt = @()
try {
    foreach ($runtime in $pin.runtime) {
        $target = Join-Path $destinationRoot "runtime/$($runtime.filename)"
        if (-not (Test-Path -LiteralPath $target)) {
            $entry = $archive.GetEntry($runtime.archivePath)
            if (-not $entry) { throw "Pinned archive entry absent: $($runtime.archivePath)" }
            [IO.Directory]::CreateDirectory([IO.Path]::GetDirectoryName($target)) | Out-Null
            $inputStream = $entry.Open(); $outputStream = [IO.File]::Create($target)
            try { $inputStream.CopyTo($outputStream) } finally { $outputStream.Dispose(); $inputStream.Dispose() }
        }
        Assert-Hash $target $runtime.sha256
        $bytes = [IO.File]::ReadAllBytes($target)
        if ($bytes.Length -ne $runtime.bytes -or $bytes.Length -lt 64 -or [BitConverter]::ToUInt16($bytes,0) -ne 0x5a4d) { throw "Invalid pinned PE: $target" }
        $peOffset = [BitConverter]::ToInt32($bytes,0x3c)
        if ($peOffset -lt 64 -or $peOffset -gt $bytes.Length-24 -or [BitConverter]::ToUInt32($bytes,$peOffset) -ne 0x4550 -or [BitConverter]::ToUInt16($bytes,$peOffset+4) -ne 0x8664) { throw "FSR runtime must be x64 PE: $target" }
        $signature = Get-AuthenticodeSignature -LiteralPath $target
        if ($signature.Status -ne 'Valid' -or $signature.SignerCertificate.Thumbprint -ne $runtime.signerThumbprint) { throw "Pinned AMD signature could not be verified: $target ($($signature.Status))" }
        $version = [Diagnostics.FileVersionInfo]::GetVersionInfo($target)
        if ($version.FileVersion -ne $runtime.fileVersion) { throw "Pinned runtime version differs: $target" }
        $receipt += [ordered]@{filename=$runtime.filename;sha256=$runtime.sha256;machine='0x8664';fileVersion=$version.FileVersion;productVersion=$version.ProductVersion;signatureStatus=[string]$signature.Status;signer=$signature.SignerCertificate.Subject;signerThumbprint=$signature.SignerCertificate.Thumbprint}
    }
} finally { $archive.Dispose() }
[ordered]@{sdkRelease=$pin.release;commit=$pin.commit;archiveSha256=$pin.archive.sha256;headers=$pin.headers;runtime=$receipt;license='Kits/FidelityFX/docs/license.md';provider='Not queried; SDK/runtime version does not identify active SR provider'} | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath (Join-Path $destinationRoot 'acquisition-receipt.json') -Encoding UTF8
Write-Output "Verified pinned FSR SR headers/runtime: $destinationRoot"
