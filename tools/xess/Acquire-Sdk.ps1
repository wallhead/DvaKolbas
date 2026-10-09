[CmdletBinding()]
param(
    [string]$Destination = (Join-Path $PSScriptRoot '../../out/research/xess-sdk-3.0.2'),
    [string]$Aio19Archive,
    [string]$SevenZip = 'C:/Program Files/7-Zip/7z.exe'
)
$ErrorActionPreference = 'Stop'
$pin = Get-Content -LiteralPath (Join-Path $PSScriptRoot 'sdk-pin.json') -Raw | ConvertFrom-Json
$root = [IO.Path]::GetFullPath($Destination)
function Assert-Hash([string]$Path, [string]$Expected) {
    if ((Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant() -ne $Expected) {
        throw "XeSS SHA256 mismatch: $Path"
    }
}
function Get-PinnedFile([string]$Relative, [string]$Target, [string]$Hash) {
    if (Test-Path -LiteralPath $Target) { Assert-Hash $Target $Hash; return }
    [IO.Directory]::CreateDirectory([IO.Path]::GetDirectoryName($Target)) | Out-Null
    $part = "$Target.$([guid]::NewGuid().ToString('N')).part"
    try {
        Invoke-WebRequest -Uri "https://raw.githubusercontent.com/intel/xess/$($pin.commit)/$Relative" -OutFile $part
        Assert-Hash $part $Hash
        Move-Item -LiteralPath $part -Destination $Target
    } finally {
        if (Test-Path -LiteralPath $part) { Remove-Item -LiteralPath $part }
    }
}
foreach ($file in $pin.files) {
    Get-PinnedFile $file.path (Join-Path $root $file.path) $file.sha256
}
$runtime = Join-Path $root $pin.runtime.stagePath
if ((Test-Path -LiteralPath $runtime) -or -not $Aio19Archive) {
    Get-PinnedFile $pin.runtime.path $runtime $pin.runtime.sha256
} else {
    Assert-Hash $Aio19Archive $pin.aio19.sha256
    [IO.Directory]::CreateDirectory([IO.Path]::GetDirectoryName($runtime)) | Out-Null
    # Extract exactly one entry. Hash before making the payload available to the loader.
    $part = "$runtime.$([guid]::NewGuid().ToString('N')).part"
    $start = [Diagnostics.ProcessStartInfo]::new($SevenZip)
    $start.UseShellExecute = $false
    $start.CreateNoWindow = $true
    $start.RedirectStandardOutput = $true
    foreach ($arg in @('x','-so',$Aio19Archive,$pin.aio19.entry.Replace('/','\'))) { $start.ArgumentList.Add($arg) }
    $process = [Diagnostics.Process]::Start($start)
    $output = [IO.File]::Create($part)
    try {
        $process.StandardOutput.BaseStream.CopyTo($output)
        $output.Dispose()
        $process.WaitForExit()
        if ($process.ExitCode -ne 0) { throw 'AIO19 one-file extraction failed' }
        Assert-Hash $part $pin.runtime.sha256
        Move-Item -LiteralPath $part -Destination $runtime
    } finally {
        $output.Dispose(); $process.Dispose()
        if (Test-Path -LiteralPath $part) { Remove-Item -LiteralPath $part }
    }
}
if ((Get-Item -LiteralPath $runtime).Length -ne $pin.runtime.bytes) { throw 'Unexpected XeSS DLL length' }
[ordered]@{ sdkRelease=$pin.release; commit=$pin.commit; runtime=$pin.runtime; files=$pin.files } |
    ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $root 'acquisition-receipt.json') -Encoding utf8
Write-Output "PASS: minimal pinned XeSS SDK acquired: $root"
