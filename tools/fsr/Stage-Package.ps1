param([Parameter(Mandatory)][ValidateSet('Standard','Universal')][string]$Edition,[Parameter(Mandatory)][string]$BuildDirectory,[Parameter(Mandatory)][string]$RuntimeDirectory,[Parameter(Mandatory)][string]$OutputDirectory,[switch]$FrameGeneration)
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
$fgBuilt=($cache -match '(?m)^TRP_ENABLE_FSR_FG:BOOL=ON\r?$')
if($FrameGeneration -and -not $fgBuilt){throw 'FG build capability required'}
$identity=Get-EmbeddedBuildIdentity (Join-Path $build 'Release/RaZkolbaS.dll')
if($identity.edition -ne $Edition -or -not $identity.fsrCompiled -or $identity.frameGenerationCompiled -ne $fgBuilt -or $identity.neuralRenderingCompiled -ne $nr){throw 'Compiled DLL capability differs from build cache'}
$runtimePins=@($pin.runtime);$headerPins=@($pin.headers)
if($FrameGeneration){$fgPin=Get-Content -LiteralPath (Join-Path $PSScriptRoot 'fg-runtime-pin.json') -Raw|ConvertFrom-Json;$runtimePins+=@($fgPin.runtime);$headerPins+=@($fgPin.headers)}
$kind=if($FrameGeneration){'FG'}else{'SR'}
foreach($file in $runtimePins){Assert-PinnedFile (Join-Path $runtimeRoot $file.filename) $file.sha256 $file.bytes}
foreach($file in $headerPins){Assert-PinnedFile (Join-Path $sdkRoot $file.path) $file.sha256}
$destination=Join-Path ([IO.Path]::GetFullPath($OutputDirectory)) ($Edition+'-FSR-'+$kind)
$zip=$destination+'.zip'
if(Test-Path -LiteralPath $destination){throw "Staging destination already exists: $destination"}
if(Test-Path -LiteralPath $zip){throw "ZIP already exists: $zip"}
[IO.Directory]::CreateDirectory((Join-Path $destination 'SKSE/Plugins/FSR')) | Out-Null
[IO.Directory]::CreateDirectory((Join-Path $destination 'SKSE/Plugins/RaZkolbaS')) | Out-Null
Copy-Item -LiteralPath (Join-Path $build 'Release/RaZkolbaS.dll') -Destination (Join-Path $destination 'SKSE/Plugins/RaZkolbaS.dll')
Copy-Item -LiteralPath (Join-Path $repository ('package/examples/FSR-'+$kind+'/RaZkolbaS.ini')) -Destination (Join-Path $destination 'SKSE/Plugins/RaZkolbaS.ini')
if(-not $FrameGeneration){
    # SR-only diagnostic builds have no FG presenter; normal releases retain backend 2.
    $diagnosticIni=Join-Path $destination 'SKSE/Plugins/RaZkolbaS.ini'
    $diagnosticLines=Set-PackageIniValues ([IO.File]::ReadAllLines($diagnosticIni)) @{'Upscaling Advanced/FsrOrdinaryPresenter'='true'}
    [IO.File]::WriteAllLines($diagnosticIni,$diagnosticLines,[Text.UTF8Encoding]::new($false))
}
Copy-Item -LiteralPath (Join-Path $repository 'package/SKSE/Plugins/RaZkolbaSImGui.ini') -Destination (Join-Path $destination 'SKSE/Plugins/RaZkolbaSImGui.ini')
Copy-Item -LiteralPath (Join-Path $repository 'package/SKSE/Plugins/RaZkolbaS/RCAS.hlsl') -Destination (Join-Path $destination 'SKSE/Plugins/RaZkolbaS/RCAS.hlsl')
foreach($file in $runtimePins){Copy-Item -LiteralPath (Join-Path $runtimeRoot $file.filename) -Destination (Join-Path $destination 'SKSE/Plugins/FSR')}
foreach($pair in @(@('LICENSE','LICENSE'),@('package/THIRD_PARTY_FSR.md','THIRD_PARTY_FSR.md'),@('package/FSR-API-MIT-NOTICE.txt','FSR-API-MIT-NOTICE.txt'),@('docs/FSR_TEST_CHECKLIST.md','FSR_TEST_CHECKLIST.md'))) {Copy-Item -LiteralPath (Join-Path $repository $pair[0]) -Destination (Join-Path $destination $pair[1])}
$description=if($FrameGeneration){'This package enables analytical FSR Super Resolution and AMD frame generation on presenter backend 2. One generated frame is requested per eligible temporal source. Native UI is dedicated; menus, loading, slow sources and FG off retain real frames. Neural Rendering, HDR and dynamic resolution are unavailable. Changing presenter needs restart; FG on/off is live. Automatic-compositor appearance and physical cadence require controlled visible acceptance before a Skyrim trial.'}else{'This package enables FSR Super Resolution with ordinary presentation. Frame generation, Neural Rendering, HDR and dynamic resolution are unavailable in this configuration.'}
$moduleCount=if($FrameGeneration){'three'}else{'two'}
@"
# RaZkolbaS — $Edition FSR $kind test build

$description

Quality, provider policy, SourceColorEncoding and sharpness are configured under [FSR] in SKSE/Plugins/RaZkolbaS.ini. Quality/provider/mode/encoding changes need Save and restart; sharpness can change live. SourceColorEncoding must explicitly be Linear, Gamma22 or SRGB; missing/Unknown encoding prevents FSR startup. The example's Gamma22 value is provisional and requires installed Skyrim/ENB color calibration. The Image tab reports waiting, active temporal FSR, spatial recovery or failure separately.

Install as a separate mod only after authorization, with Skyrim and MO2 closed. Target profile: V5.4 NO-LORE. Preserve the working DLSS mod for rollback. The user starts Skyrim manually through the existing MO2 SKSE entry. This package has been staged and validated; Skyrim gameplay acceptance remains open.

See [FSR_TEST_CHECKLIST.md](FSR_TEST_CHECKLIST.md) for launch and image/lifecycle checks, and [THIRD_PARTY_FSR.md](THIRD_PARTY_FSR.md) for binary/license identities. manifest.json records every file hash. The $moduleCount AMD runtimes are in SKSE/Plugins/FSR. No NVIDIA runtime DLL is included or required by the FSR route; Microsoft C++ runtime and a compatible D3D11.4/D3D12/SM6 graphics driver are required.

Corresponding source: https://github.com/wallhead/RaZkolbaS/tree/codex/fsr-sr
"@ | Set-Content -LiteralPath (Join-Path $destination 'README.md') -Encoding utf8
Copy-Item -LiteralPath (Join-Path $sdkRoot 'Kits/FidelityFX/docs/license.md') -Destination (Join-Path $destination 'AMD-FidelityFX-license.md')
$files=@(Get-ChildItem -LiteralPath $destination -Recurse -File | Sort-Object FullName | ForEach-Object {[ordered]@{path=[IO.Path]::GetRelativePath($destination,$_.FullName).Replace('\','/');bytes=$_.Length;sha256=(Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant()}})
$dllImports=[ordered]@{}
foreach($file in Get-ChildItem -LiteralPath $destination -Recurse -Filter '*.dll'){$relative=[IO.Path]::GetRelativePath($destination,$file.FullName).Replace('\','/');$dllImports[$relative]=@(Get-PEImports $file.FullName)}
$providerVersions=@()
if($FrameGeneration){$providerVersions=@($fgPin.observedProviders)}
[ordered]@{schema=2;edition=$Edition;fsrEnabled=$true;neuralRenderingCompiled=$nr;frameGenerationImplemented=[bool]$FrameGeneration;frameGenerationCompiled=$fgBuilt;buildIdentity=$identity;dllImports=$dllImports;providerVersions=$providerVersions;sdkRelease=$pin.release;sdkCommit=$pin.commit;files=$files} | ConvertTo-Json -Depth 10 | Set-Content -LiteralPath (Join-Path $destination 'manifest.json') -Encoding utf8
& (Join-Path $PSScriptRoot 'Validate-Package.ps1') -Edition $Edition -PackageDirectory $destination -FrameGeneration:$FrameGeneration
Add-Type -AssemblyName System.IO.Compression.FileSystem
[IO.Compression.ZipFile]::CreateFromDirectory($destination,$zip,[IO.Compression.CompressionLevel]::Optimal,$false)
Write-Output "STAGED (not installed): $zip"
