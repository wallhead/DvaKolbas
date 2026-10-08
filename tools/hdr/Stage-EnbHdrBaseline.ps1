param(
    [Parameter(Mandatory)][string]$ReleaseArchive,
    [Parameter(Mandatory)][string]$SourceIni,
    [Parameter(Mandatory)][string]$OutputArchive,
    [Parameter(Mandatory)][string]$StagingDirectory,
    [ValidateRange(80,10000)][int]$PeakNits = 400,
    [string]$SevenZip = 'C:/Program Files/7-Zip/7z.exe'
)
$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot '../nr/RuntimePackageCommon.ps1')
Add-Type -AssemblyName System.IO.Compression.FileSystem
$release = [IO.Path]::GetFullPath($ReleaseArchive)
$output = [IO.Path]::GetFullPath($OutputArchive)
$stage = [IO.Path]::GetFullPath($StagingDirectory)
Assert-PinnedFile $release 'e9be721ab59269bafaa31ad501fa969aea02856259ddb8708ba597b6efe2d9d3' 367376145
if (Test-Path -LiteralPath $stage) { throw 'HDR stage already exists' }
if (Test-Path -LiteralPath $output) { throw 'HDR archive already exists' }
if ([IO.Path]::GetExtension($output) -ne '.zip') { throw 'HDR output must be a ZIP archive' }
$plugins = Join-Path $stage 'SKSE/Plugins'
[IO.Directory]::CreateDirectory($plugins) | Out-Null
$iniPath = Join-Path $plugins 'RaZkolbaS.ini'
$lines = ConvertTo-PortableNrPackageIni ([IO.File]::ReadAllLines([IO.Path]::GetFullPath($SourceIni)))
$lines = Set-PackageIniValues $lines @{
    'Upscaling/Upscaler'='DLSS'; 'DLSS/Quality'='Native';
    'FrameGeneration/Backend'='NVIDIA'; 'FrameGeneration/Enabled'='false';
    'NeuralRendering/Enabled'='false'; 'HDROutput/Enabled'='true';
    'HDROutput/MatchWindowsSDRBrightness'='true'; 'HDROutput/PeakNits'=[string]$PeakNits;
    'HDROutput/SDRTransfer'='Gamma22'; 'Interface/NativeUI'='true';
    'Interface/UIComposition'='Dedicated'; 'DynamicResolution/Enabled'='false';
    'DynamicResolution/Oscillate'='false'
    'Runtime/StreamlineDirectory'='RaZkolbaS/NVIDIA/Streamline'
}
[IO.File]::WriteAllLines($iniPath,[string[]]$lines,[Text.UTF8Encoding]::new($false))
$check = Read-PackageIni $iniPath -Raw
foreach ($setting in @{
    'Upscaling/Upscaler'='DLSS'; 'DLSS/Quality'='Native';
    'FrameGeneration/Backend'='NVIDIA'; 'FrameGeneration/Enabled'='false';
    'NeuralRendering/Enabled'='false'; 'HDROutput/Enabled'='true';
    'HDROutput/MatchWindowsSDRBrightness'='true'; 'HDROutput/PeakNits'=[string]$PeakNits;
    'HDROutput/SDRTransfer'='Gamma22'; 'Interface/NativeUI'='true'; 'Interface/UIComposition'='Dedicated';
    'Runtime/StreamlineDirectory'='RaZkolbaS/NVIDIA/Streamline'
}.GetEnumerator()) {
    if ($check[$setting.Key] -cne $setting.Value) { throw "HDR setting mismatch: $($setting.Key)" }
}
if ($check['Runtime/NRDriverCore'] -or $check['Runtime/NRRuntimeRoot']) { throw 'Nonportable NR runtime override' }
$base = [IO.Compression.ZipFile]::OpenRead($release)
try {
    $meta = $base.GetEntry('meta.ini')
    if (-not $meta) { throw 'Release metadata absent' }
    $reader = [IO.StreamReader]::new($meta.Open())
    try { $metadata = $reader.ReadToEnd() } finally { $reader.Dispose() }
    $metadata = $metadata -replace '(?m)^version=.*$', 'version=1.3-hdr-baseline'
    $metadata = $metadata -replace '(?m)^installationFile=.*$', ('installationFile='+[IO.Path]::GetFileName($output))
    $metadata = $metadata -replace '(?m)^notes=.*$', 'notes=Experimental ENB HDR baseline; DLAA; NVIDIA presentation; FG off; NR off; Windows HDR required; peak brightness provisional.'
    [IO.File]::WriteAllText((Join-Path $stage 'meta.ini'),$metadata,[Text.UTF8Encoding]::new($false))
    Copy-Item -LiteralPath $release -Destination $output
    Push-Location -LiteralPath $stage
    try {
        & $SevenZip u -tzip $output 'meta.ini' 'SKSE/Plugins/RaZkolbaS.ini' -mx=9 -bd | Out-Null
        if ($LASTEXITCODE -ne 0) { throw 'HDR archive update failed' }
    } finally { Pop-Location }
    & $SevenZip t $output -bd | Out-Null
    if ($LASTEXITCODE -ne 0) { throw 'HDR archive CRC failed' }
    $trial = [IO.Compression.ZipFile]::OpenRead($output)
    try {
        if ($trial.Entries.Count -ne $base.Entries.Count) { throw 'Unexpected HDR inventory change' }
        $files = @()
        foreach ($entry in $trial.Entries) {
            if ($entry.FullName -match '\\|(^|/)\.\.?(/|$)' -or
                ($entry.FullName -ne 'meta.ini' -and -not $entry.FullName.StartsWith('SKSE/'))) { throw 'Unexpected HDR archive path' }
            $sha = [Security.Cryptography.SHA256]::Create()
            $stream = $entry.Open()
            try { $hash = [BitConverter]::ToString($sha.ComputeHash($stream)).Replace('-','').ToLowerInvariant() }
            finally { $stream.Dispose(); $sha.Dispose() }
            if ($entry.FullName -in @('meta.ini','SKSE/Plugins/RaZkolbaS.ini')) {
                $expected = (Get-FileHash -LiteralPath (Join-Path $stage $entry.FullName) -Algorithm SHA256).Hash.ToLowerInvariant()
            } else {
                $original = $base.GetEntry($entry.FullName)
                if (-not $original -or $original.Length -ne $entry.Length) { throw 'HDR runtime inventory differs' }
                $sha = [Security.Cryptography.SHA256]::Create()
                $stream = $original.Open()
                try { $expected = [BitConverter]::ToString($sha.ComputeHash($stream)).Replace('-','').ToLowerInvariant() }
                finally { $stream.Dispose(); $sha.Dispose() }
            }
            if ($hash -ne $expected) { throw "HDR payload mismatch: $($entry.FullName)" }
            $files += [ordered]@{path=$entry.FullName; bytes=$entry.Length; sha256=$hash}
        }
    } finally { $trial.Dispose() }
} finally { $base.Dispose() }
[ordered]@{
    scope='Staged only; not installed or gameplay qualified'; baseline='ENB SDR to HDR10';
    archive=[IO.Path]::GetFileName($output); archiveSha256=(Get-FileHash -LiteralPath $output -Algorithm SHA256).Hash.ToLowerInvariant();
    sourceIniSha256=(Get-FileHash -LiteralPath $SourceIni -Algorithm SHA256).Hash.ToLowerInvariant();
    binarySourceRevision='70edb4c1fabc'; dllUnchanged=$true; runtimesUnchanged=$true; crcVerified=$true;
    upscaler='DLAA'; fgBackend='NVIDIA'; fgEnabled=$false; nrEnabled=$false; hdrEnabled=$true;
    peakNits=$PeakNits; peakBrightnessCalibrated=$false; files=$files
} | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath (Join-Path $stage 'hdr-baseline-verification.json') -Encoding utf8
Write-Output "PASS: ENB HDR baseline; DLAA; FG/NR off; $($files.Count) archive entries verified; plugin/runtimes unchanged; not installed"
