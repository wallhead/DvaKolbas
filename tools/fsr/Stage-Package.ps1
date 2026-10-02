param([Parameter(Mandatory)][ValidateSet('Standard','Universal')][string]$Edition,[Parameter(Mandatory)][string]$BuildDirectory,[Parameter(Mandatory)][string]$RuntimeDirectory,[Parameter(Mandatory)][string]$OutputDirectory)
. (Join-Path $PSScriptRoot 'PackageCommon.ps1')
$repository=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$build=[IO.Path]::GetFullPath($BuildDirectory);$runtimeRoot=[IO.Path]::GetFullPath($RuntimeDirectory)
$sdkRoot=[IO.Path]::GetFullPath((Join-Path $runtimeRoot '..'))
$pin=Get-Content -LiteralPath (Join-Path $PSScriptRoot 'runtime-pin.json') -Raw | ConvertFrom-Json
$cache=Get-Content -LiteralPath (Join-Path $build 'CMakeCache.txt') -Raw
if($cache -notmatch '(?m)^TRP_ENABLE_FSR:BOOL=ON\r?$'){throw 'Build lacks FSR'}
$features=if($Edition -eq 'Universal'){'ON'}else{'OFF'}
if($cache -notmatch "(?m)^TRP_ENABLE_OPTIONAL_FEATURES:BOOL=$features\r?`$"){throw 'Build edition mismatch'}
$nr=($cache -match '(?m)^TRP_ENABLE_NEURAL_RENDERING:BOOL=ON\r?$')
foreach($file in $pin.runtime){Assert-PinnedFile (Join-Path $runtimeRoot $file.filename) $file.sha256 $file.bytes}
foreach($file in $pin.headers){Assert-PinnedFile (Join-Path $sdkRoot $file.path) $file.sha256}
$destination=Join-Path ([IO.Path]::GetFullPath($OutputDirectory)) ($Edition+'-FSR-SR')
$zip=$destination+'.zip'
if(Test-Path -LiteralPath $destination){throw "Staging destination already exists: $destination"}
if(Test-Path -LiteralPath $zip){throw "ZIP already exists: $zip"}
[IO.Directory]::CreateDirectory((Join-Path $destination 'SKSE/Plugins/FSR')) | Out-Null
[IO.Directory]::CreateDirectory((Join-Path $destination 'SKSE/Plugins/TheosRenderPipeline')) | Out-Null
Copy-Item -LiteralPath (Join-Path $build 'Release/TheosRenderPipeline.dll') -Destination (Join-Path $destination 'SKSE/Plugins/TheosRenderPipeline.dll')
Copy-Item -LiteralPath (Join-Path $repository 'package/examples/FSR-SR/TheosRenderPipeline.ini') -Destination (Join-Path $destination 'SKSE/Plugins/TheosRenderPipeline.ini')
Copy-Item -LiteralPath (Join-Path $repository 'package/SKSE/Plugins/TheosRenderPipelineImGui.ini') -Destination (Join-Path $destination 'SKSE/Plugins/TheosRenderPipelineImGui.ini')
Copy-Item -LiteralPath (Join-Path $repository 'package/SKSE/Plugins/TheosRenderPipeline/RCAS.hlsl') -Destination (Join-Path $destination 'SKSE/Plugins/TheosRenderPipeline/RCAS.hlsl')
foreach($file in $pin.runtime){Copy-Item -LiteralPath (Join-Path $runtimeRoot $file.filename) -Destination (Join-Path $destination 'SKSE/Plugins/FSR')}
foreach($pair in @(@('LICENSE','LICENSE'),@('package/THIRD_PARTY_FSR.md','THIRD_PARTY_FSR.md'),@('package/FSR-API-MIT-NOTICE.txt','FSR-API-MIT-NOTICE.txt'),@('docs/FSR_TEST_CHECKLIST.md','FSR_TEST_CHECKLIST.md'))) {Copy-Item -LiteralPath (Join-Path $repository $pair[0]) -Destination (Join-Path $destination $pair[1])}
@"
# Theo's Render Pipeline — $Edition FSR SR test build

This package enables FSR Super Resolution with ordinary presentation. FSR frame generation, Neural Rendering, HDR and dynamic resolution are unavailable in this configuration.

Quality, provider policy and sharpness are configured under [FSR] in SKSE/Plugins/TheosRenderPipeline.ini. Quality/provider/mode changes need Save and restart; sharpness can change live. The Image tab reports waiting, active temporal FSR, spatial recovery or failure separately.

Install as a separate mod only after authorization, with Skyrim and MO2 closed. Target profile: V5.4 NO-LORE. Preserve the working DLSS mod for rollback. The user starts Skyrim manually through the existing MO2 SKSE entry. This package has been staged and validated; Skyrim gameplay acceptance remains open.

See [FSR_TEST_CHECKLIST.md](FSR_TEST_CHECKLIST.md) for launch and image/lifecycle checks, and [THIRD_PARTY_FSR.md](THIRD_PARTY_FSR.md) for binary/license identities. manifest.json records every file hash. The two AMD runtimes are in SKSE/Plugins/FSR. No NVIDIA runtime DLL is included or required by the FSR route; Microsoft C++ runtime and a compatible D3D11.4/D3D12/SM6 graphics driver are required.

Corresponding source: https://github.com/wallhead/DvaKolbas/tree/codex/fsr-sr
"@ | Set-Content -LiteralPath (Join-Path $destination 'README.md') -Encoding utf8
Copy-Item -LiteralPath (Join-Path $sdkRoot 'Kits/FidelityFX/docs/license.md') -Destination (Join-Path $destination 'AMD-FidelityFX-license.md')
$files=@(Get-ChildItem -LiteralPath $destination -Recurse -File | Sort-Object FullName | ForEach-Object {[ordered]@{path=[IO.Path]::GetRelativePath($destination,$_.FullName).Replace('\','/');bytes=$_.Length;sha256=(Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant()}})
[ordered]@{schema=1;edition=$Edition;fsrEnabled=$true;neuralRenderingCompiled=$nr;frameGenerationImplemented=$false;sdkRelease=$pin.release;sdkCommit=$pin.commit;files=$files} | ConvertTo-Json -Depth 10 | Set-Content -LiteralPath (Join-Path $destination 'manifest.json') -Encoding utf8
& (Join-Path $PSScriptRoot 'Validate-Package.ps1') -Edition $Edition -PackageDirectory $destination
Add-Type -AssemblyName System.IO.Compression.FileSystem
[IO.Compression.ZipFile]::CreateFromDirectory($destination,$zip,[IO.Compression.CompressionLevel]::Optimal,$false)
Write-Output "STAGED (not installed): $zip"
