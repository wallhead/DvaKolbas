param(
    [Parameter(Mandatory)][string]$TemplateArchive,
    [Parameter(Mandatory)][string]$PluginDll,
    [Parameter(Mandatory)][string]$Int8RuntimeDirectory,
    [Parameter(Mandatory)][string]$StagingDirectory,
    [Parameter(Mandatory)][string]$OutputArchive,
    [switch]$MlFgTrial,
    [string]$SevenZip='C:/Program Files/7-Zip/7z.exe'
)
$ErrorActionPreference='Stop'
. (Join-Path $PSScriptRoot '../nr/RuntimePackageCommon.ps1')
Add-Type -AssemblyName System.IO.Compression.FileSystem
$stage=[IO.Path]::GetFullPath($StagingDirectory)
$archive=[IO.Path]::GetFullPath($OutputArchive)
if((Test-Path -LiteralPath $stage) -or (Test-Path -LiteralPath $archive)){throw 'Candidate output already exists'}
$identity=Get-EmbeddedBuildIdentity $PluginDll
if(-not $identity.sourceClean -or $identity.edition -ne 'Universal' -or -not $identity.fsrCompiled -or
    -not $identity.frameGenerationCompiled -or -not $identity.neuralRenderingCompiled){throw 'Clean Universal FSR/FG/NR build required'}
$int8Pin=Get-Content -LiteralPath (Join-Path $PSScriptRoot 'int8-runtime-pin.json') -Raw | ConvertFrom-Json
foreach($file in $int8Pin.runtime){Assert-PinnedFile (Join-Path $Int8RuntimeDirectory $file.filename) $file.sha256 $file.bytes}
$template=[IO.Compression.ZipFile]::OpenRead([IO.Path]::GetFullPath($TemplateArchive))
try {
    foreach($entry in $template.Entries){
        if($entry.FullName -match '\\|(^|/)\.\.?(/|$)' -or
            ($entry.FullName -ne 'meta.ini' -and -not $entry.FullName.StartsWith('SKSE/'))){throw "Unexpected template path: $($entry.FullName)"}
    }
} finally {$template.Dispose()}
[IO.Compression.ZipFile]::ExtractToDirectory([IO.Path]::GetFullPath($TemplateArchive),$stage)
$srPin=Get-Content -LiteralPath (Join-Path $PSScriptRoot 'runtime-pin.json') -Raw | ConvertFrom-Json
$fgPin=Get-Content -LiteralPath (Join-Path $PSScriptRoot 'fg-runtime-pin.json') -Raw | ConvertFrom-Json
foreach($file in @($srPin.runtime)+@($fgPin.runtime)){
    Assert-PinnedFile (Join-Path $stage ('SKSE/Plugins/FSR/'+$file.filename)) $file.sha256 $file.bytes
}
$plugins=Join-Path $stage 'SKSE/Plugins'
$int8=Join-Path $plugins 'FSR/INT8'
[IO.Directory]::CreateDirectory($int8) | Out-Null
foreach($file in $int8Pin.runtime){Copy-Item -LiteralPath (Join-Path $Int8RuntimeDirectory $file.filename) -Destination $int8}
Copy-Item -LiteralPath $PluginDll -Destination (Join-Path $plugins 'RaZkolbaS.dll')
$iniPath=Join-Path $plugins 'RaZkolbaS.ini'
$lines=ConvertTo-PortableNrPackageIni ([IO.File]::ReadAllLines($iniPath))
$srPolicy=if($MlFgTrial){'Compatible'}else{'MachineLearning'}
$fgPolicy=if($MlFgTrial){'MachineLearning'}else{'Analytical'}
$lines=Set-PackageIniValues $lines @{'Settings/UpscaleType'='4';'FSR/Quality'='NativeAA';'FSR/ProviderPolicy'=$srPolicy;
    'FrameGeneration/FsrProviderPolicy'=$fgPolicy;'FrameGeneration/Backend'='2';'FrameGeneration/Enabled'='false';'NeuralRendering/Enabled'='false'}
$lines=@($lines | ForEach-Object {
    if($_ -match '^; (FSR4|Official FSR4|Startup:.*Analytical).*') {
        '; Provider: Analytical=FSR 3.1.5; MachineLearning=FSR4; save and restart to switch.'
        '; NVIDIA FSR4 uses FSR/INT8 (SM6.6). Auto retains the official FSR runtime.'
    } else {$_}
})
[IO.File]::WriteAllLines($iniPath,[string[]]$lines,[Text.UTF8Encoding]::new($false))
$ini=Read-PortableNrPackageIni $iniPath
if($ini['Settings/UpscaleType'] -ne '4' -or $ini['FSR/Quality'] -ne 'NativeAA' -or
   $ini['FSR/ProviderPolicy'] -ne $srPolicy -or $ini['FrameGeneration/FsrProviderPolicy'] -ne $fgPolicy -or $ini['Experimental/FrameGenerationBackend'] -ne '2' -or
   $ini['FrameGeneration/Enabled'] -ne 'false' -or $ini['NeuralRendering/Enabled'] -ne 'false'){throw 'Trial defaults differ from intended first-launch settings'}
Write-PortableModMetadata -Directory $stage -Revision $identity.sourceRevision
$meta=Join-Path $stage 'meta.ini'
$previewVersion=if($MlFgTrial){'1.1-fsr4-fg-preview'}else{'1.1-fsr4-preview'}
$previewNote=if($MlFgTrial){'Experimental FSR4 FG; RX9000 standalone and Skyrim qualification pending.'}else{'FSR4 NVIDIA preview; Skyrim acceptance pending.'}
$metaText=[regex]::Replace([IO.File]::ReadAllText($meta),'(?m)^version=.*$',('version='+$previewVersion))
$metaText=[regex]::Replace($metaText,'(?m)^installationFile=.*$',('installationFile='+[IO.Path]::GetFileName($archive)))
$metaText=[regex]::Replace($metaText,'(?m)^notes=.*$',('notes=Build '+$identity.sourceRevision+'; '+$previewNote))
[IO.File]::WriteAllText($meta,$metaText,[Text.UTF8Encoding]::new($false))
Push-Location -LiteralPath $stage
try {& $SevenZip a -tzip $archive 'meta.ini' 'SKSE' -mx=5 -bd | Out-Null;if($LASTEXITCODE -ne 0){throw 'Candidate archive creation failed'}}
finally {Pop-Location}
& $SevenZip t $archive -bd | Out-Null
if($LASTEXITCODE -ne 0){throw 'Candidate archive CRC failure'}
$packed=[IO.Compression.ZipFile]::OpenRead($archive)
try {
    $files=@()
    foreach($entry in $packed.Entries){
        if($entry.FullName.EndsWith('/')){continue}
        $path=Join-Path $stage $entry.FullName
        if(-not (Test-Path -LiteralPath $path -PathType Leaf)){throw "Unexpected packed file: $($entry.FullName)"}
        $sha=[Security.Cryptography.SHA256]::Create();$stream=$entry.Open()
        try {$hash=[BitConverter]::ToString($sha.ComputeHash($stream)).Replace('-','').ToLowerInvariant()}
        finally {$stream.Dispose();$sha.Dispose()}
        if($entry.Length -ne (Get-Item -LiteralPath $path).Length -or $hash -ne (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToLowerInvariant()){throw "Packed content mismatch: $($entry.FullName)"}
        $files+=[ordered]@{path=$entry.FullName;bytes=$entry.Length;sha256=$hash}
    }
    if($files.Count -ne @(Get-ChildItem -LiteralPath $stage -Recurse -File).Count){throw 'Candidate file inventory mismatch'}
} finally {$packed.Dispose()}
[ordered]@{result='PASS';buildIdentity=$identity;archiveName=[IO.Path]::GetFileName($archive);
    archiveSha256=(Get-FileHash -LiteralPath $archive -Algorithm SHA256).Hash.ToLowerInvariant();files=$files;
    defaults=@{mode='FSR';quality='NativeAA';provider=$srPolicy;fgProvider=$fgPolicy;fgBackend=2;fgEnabled=$false;nrEnabled=$false};
    retainsOfficialFsr3=$true;separatePinnedInt8=$true;skyrimQualified=$false;installed=$false} |
    ConvertTo-Json -Depth 8 | Set-Content -LiteralPath ($archive+'.verification.json') -Encoding utf8
Write-Output 'PASS: separate MO2 FSR4 trial; official FSR3 retained; archive payloads and defaults verified. Not installed.'
