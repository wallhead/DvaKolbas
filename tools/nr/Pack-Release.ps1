param(
    [Parameter(Mandatory)][string]$TemplateArchive,
    [Parameter(Mandatory)][string]$PluginDll,
    [Parameter(Mandatory)][string]$OutputArchive,
    [Parameter(Mandatory)][string]$StagingDirectory,
    [string]$SevenZip = 'C:/Program Files/7-Zip/7z.exe'
)
$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'RuntimePackageCommon.ps1')
Add-Type -AssemblyName System.IO.Compression.FileSystem
$template = [IO.Path]::GetFullPath($TemplateArchive)
$output = [IO.Path]::GetFullPath($OutputArchive)
$stage = [IO.Path]::GetFullPath($StagingDirectory)
if (Test-Path -LiteralPath $output) { throw 'Release archive already exists' }
if (Test-Path -LiteralPath $stage) { throw 'Release staging directory already exists' }
if ([IO.Path]::GetFileName($output) -cne 'RaZKolbaS DLSS FSR FG NR v1.3.2.zip') { throw 'Release archive name mismatch' }
$identity = Get-EmbeddedBuildIdentity $PluginDll
if ($identity.edition -ne 'Universal') { throw 'Universal renderer required for RTX 20/30 compatibility in the all-GPU release' }
if (-not $identity.sourceClean -or
    -not $identity.fsrCompiled -or -not $identity.frameGenerationCompiled -or
    -not $identity.neuralRenderingCompiled) { throw 'Expected a clean Universal build with FSR/FG/NR' }
if ([Diagnostics.FileVersionInfo]::GetVersionInfo($PluginDll).FileVersion -ne '1.3.2.0') { throw 'Plugin version must be 1.3.2.0' }
$plugins = Join-Path $stage 'SKSE/Plugins'
[IO.Directory]::CreateDirectory($plugins) | Out-Null
$iniPath = Join-Path $plugins 'RaZkolbaS.ini'
$baseZip = [IO.Compression.ZipFile]::OpenRead($template)
try {
    # The archive supplies qualified runtime assets, never an obsolete INI schema.
    $currentIni = Join-Path $PSScriptRoot '../../package/SKSE/Plugins/RaZkolbaS.ini'
    $lines = ConvertTo-PortableNrPackageIni ([IO.File]::ReadAllLines($currentIni))
    $lines = Set-PackageIniValues $lines @{
        'Settings/UpscaleType'='3';'FSR/Quality'='NativeAA';
        'FSR/SourceColorEncoding'='Gamma22';
        'FrameGeneration/Backend'='Auto';'FrameGeneration/Enabled'='false';'NeuralRendering/Enabled'='false';
        'Debug/NRLegacyRuntime'='false';'NeuralRendering/SourceColorEncoding'='Gamma22';
        'NeuralRendering/SdrBytesTrial'='true';'HDROutput/Enabled'='false'
    }
    [IO.File]::WriteAllLines($iniPath, [string[]]$lines, [Text.UTF8Encoding]::new($false))
    Copy-Item -LiteralPath $PluginDll -Destination (Join-Path $plugins 'RaZkolbaS.dll')
    Write-PortableModMetadata -Directory $stage -Revision $identity.sourceRevision
    Copy-Item -LiteralPath $template -Destination $output
    $removedAudio = @($baseZip.Entries | Where-Object {$_.FullName -match '^SKSE/Plugins/RaZkolbaS/Audio(/|$)'} | ForEach-Object {$_.FullName})
    if ($removedAudio.Count) {
        & $SevenZip d -tzip $output @removedAudio -bd
        if ($LASTEXITCODE -ne 0) { throw 'Release audio removal failed' }
    }
    Push-Location -LiteralPath $stage
    try {
        & $SevenZip u -tzip $output 'meta.ini' 'SKSE/Plugins/RaZkolbaS.ini' 'SKSE/Plugins/RaZkolbaS.dll' -mx=9 -bd
        if ($LASTEXITCODE -ne 0) { throw 'Release archive update failed' }
    } finally { Pop-Location }
    & $SevenZip t $output -bd
    if ($LASTEXITCODE -ne 0) { throw 'Release archive CRC verification failed' }
    $releaseZip = [IO.Compression.ZipFile]::OpenRead($output)
    try {
        if ($releaseZip.Entries.Count -ne $baseZip.Entries.Count - $removedAudio.Count) { throw 'Release inventory changed unexpectedly' }
        $replacements = @{
            'meta.ini' = Join-Path $stage 'meta.ini'
            'SKSE/Plugins/RaZkolbaS.ini' = $iniPath
            'SKSE/Plugins/RaZkolbaS.dll' = Join-Path $plugins 'RaZkolbaS.dll'
        }
        $files = @()
        foreach ($entry in $releaseZip.Entries) {
            $name = $entry.FullName
            if ($name -match '^SKSE/Plugins/RaZkolbaS/Audio(/|$)') { throw 'Release still contains the private audio easter egg' }
            if ($name -match '\\|(^|/)\.\.?(/|$)' -or
                ($name -ne 'meta.ini' -and -not $name.StartsWith('SKSE/'))) { throw "Unexpected archive path: $name" }
            $sha = [Security.Cryptography.SHA256]::Create()
            $stream = $entry.Open()
            try { $hash = [BitConverter]::ToString($sha.ComputeHash($stream)).Replace('-','').ToLowerInvariant() }
            finally { $stream.Dispose(); $sha.Dispose() }
            if ($replacements.ContainsKey($name)) {
                $expected = (Get-FileHash -LiteralPath $replacements[$name] -Algorithm SHA256).Hash.ToLowerInvariant()
            } else {
                $original = $baseZip.GetEntry($name)
                if (-not $original -or $original.Length -ne $entry.Length) { throw "Runtime inventory changed: $name" }
                $sha = [Security.Cryptography.SHA256]::Create()
                $stream = $original.Open()
                try { $expected = [BitConverter]::ToString($sha.ComputeHash($stream)).Replace('-','').ToLowerInvariant() }
                finally { $stream.Dispose(); $sha.Dispose() }
            }
            if ($hash -ne $expected) { throw "Release content mismatch: $name" }
            $files += [ordered]@{path=$name;bytes=$entry.Length;sha256=$hash}
        }
    } finally { $releaseZip.Dispose() }
} finally { $baseZip.Dispose() }
$ini = Read-PortableNrPackageIni $iniPath
if ($ini['Settings/UpscaleType'] -ne '3' -or $ini['FSR/Quality'] -ne 'NativeAA') { throw 'Native defaults verification failed' }
if ($ini['FrameGeneration/Backend'] -ne '1' -or $ini['FrameGeneration/Enabled'] -ne 'false' -or
    $ini['NeuralRendering/Enabled'] -ne 'false' -or (Read-PackageIni $iniPath -Raw)['FrameGeneration/Backend'] -ne 'Auto') {
    throw 'Release defaults must use Auto presentation with FG and NR disabled'
}
if ($ini['Experimental/SourceDLSSGMFGUnlock'] -ne 'true') { throw 'Release INI must enable RTX 20/30 compatibility' }
if ($ini['FSR/SourceColorEncoding'] -ne 'Gamma22' -or
    $ini['NeuralRendering/CommunityRuntime'] -ne 'true' -or
    (Read-PackageIni $iniPath -Raw).ContainsKey('NeuralRendering Advanced/Runtime') -or
    $ini['NeuralRendering/SourceColorEncoding'] -ne 'Gamma22' -or
    $ini['NeuralRendering/SdrBytesTrial'] -ne 'true' -or
    $ini['HDROutput/Enabled'] -ne 'false') {
    throw 'Release must use ENB SDR encoding, bundled Community NR and disabled HDR'
}
$receipt = [ordered]@{
    release='1.3.2'; archiveName=[IO.Path]::GetFileName($output)
    archiveSha256=(Get-FileHash -LiteralPath $output -Algorithm SHA256).Hash.ToLowerInvariant()
    archiveBytes=(Get-Item -LiteralPath $output).Length; buildIdentity=$identity
    dllVersion='1.3.2.0'; defaults=@{upscaler='DLSS';dlssQuality='Native';fsrQuality='Native';fsrSourceColorEncoding='Gamma22';fgBackend='Auto';fgEnabled=$false;nrEnabled=$false;nrRuntime='Automatic';nrSourceColorEncoding='Gamma22';nrSdrBytes=$true;hdrEnabled=$false}
    rtx20_30CompatibilityCompiled=$true; actualRtx20_30GameplayQualified=$false
    unchangedRuntimePayloads=$true; crcVerified=$true; removedAudio=$removedAudio; files=$files
}
$receipt | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath (Join-Path $stage 'release-verification.json') -Encoding utf8
Write-Output "PASS: release 1.3.2; native defaults; FG Auto/off; NR off; $($files.Count) entries verified; unchanged runtime payloads"
