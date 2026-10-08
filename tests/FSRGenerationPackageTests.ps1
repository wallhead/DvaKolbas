param([Parameter(Mandatory)][string]$BuildDirectory,[Parameter(Mandatory)][string]$RuntimeDirectory,[Parameter(Mandatory)][string]$ScratchRoot,[ValidateSet('Standard','Universal')][string]$Edition='Standard')
$ErrorActionPreference='Stop'
. (Join-Path $PSScriptRoot '../tools/fsr/PackageCommon.ps1')
$root=Join-Path ([IO.Path]::GetFullPath($ScratchRoot)) ([guid]::NewGuid().ToString('N'))
$stage=Join-Path $PSScriptRoot '../tools/fsr/Stage-Package.ps1';$validate=Join-Path $PSScriptRoot '../tools/fsr/Validate-Package.ps1'
function Require([bool]$Value,[string]$Reason){if(-not $Value){throw $Reason};Write-Output "PASS: $Reason"}
function Rejected([scriptblock]$Action,[string]$Case){$failed=$false;try{& $Action}catch{$failed=$true};Require $failed $Case}
$repository=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$defaultIni=Join-Path $repository 'package/SKSE/Plugins/RaZkolbaS.ini';$defaultHash=(Get-FileHash -LiteralPath $defaultIni).Hash
& $stage -Edition $Edition -BuildDirectory $BuildDirectory -RuntimeDirectory $RuntimeDirectory -OutputDirectory $root -FrameGeneration
$package=Join-Path $root ($Edition+'-FSR-FG');& $validate -Edition $Edition -PackageDirectory $package -FrameGeneration
$manifestPath=Join-Path $package 'manifest.json';$manifestOriginal=[IO.File]::ReadAllText($manifestPath)
$config=Join-Path $package 'SKSE/Plugins/RaZkolbaS.ini';$configOriginal=[IO.File]::ReadAllText($config)
foreach($field in @('frameGenerationCompiled','neuralRenderingCompiled','fsrEnabled')) {
 $tampered=$manifestOriginal|ConvertFrom-Json;$tampered.$field=-not $tampered.$field
 $tampered|ConvertTo-Json -Depth 12|Set-Content -LiteralPath $manifestPath
 Rejected {& $validate -Edition $Edition -PackageDirectory $package -FrameGeneration} ("CompiledMetadataCannotBeForged-"+$field)
}
[IO.File]::WriteAllText($manifestPath,$manifestOriginal)
$tampered=$manifestOriginal|ConvertFrom-Json;$tampered.providerVersions[0].name='9.9.9'
$tampered|ConvertTo-Json -Depth 12|Set-Content -LiteralPath $manifestPath
Rejected {& $validate -Edition $Edition -PackageDirectory $package -FrameGeneration} 'ProviderVersionsCannotBeForged'
[IO.File]::WriteAllText($manifestPath,$manifestOriginal)
function Update-ManifestFile([string]$Relative){$manifest=$manifestOriginal|ConvertFrom-Json;foreach($entry in $manifest.files){if($entry.path -eq $Relative){$path=Join-Path $package $Relative;$entry.bytes=(Get-Item -LiteralPath $path).Length;$entry.sha256=(Get-FileHash -LiteralPath $path).Hash.ToLowerInvariant()}};$manifest|ConvertTo-Json -Depth 12|Set-Content -LiteralPath $manifestPath}
$fg=Join-Path $package 'SKSE/Plugins/FSR/amd_fidelityfx_framegeneration_dx12.dll'
Require (@(Get-ChildItem -LiteralPath (Split-Path $fg) -Filter '*.dll').Count -eq 3) 'FG package has exactly three AMD modules'
Remove-Item -LiteralPath $fg
Rejected {& $validate -Edition $Edition -PackageDirectory $package -FrameGeneration} 'MissingFgRuntimeRejectedOnlyForFg'
Copy-Item -LiteralPath (Join-Path $RuntimeDirectory (Split-Path $fg -Leaf)) -Destination $fg
[IO.File]::AppendAllText($fg,'modified FG runtime');Update-ManifestFile 'SKSE/Plugins/FSR/amd_fidelityfx_framegeneration_dx12.dll'
Rejected {& $validate -Edition $Edition -PackageDirectory $package -FrameGeneration} 'WrongFgHashRejectedEvenWithUpdatedManifest'
Copy-Item -LiteralPath (Join-Path $RuntimeDirectory (Split-Path $fg -Leaf)) -Destination $fg
[IO.File]::WriteAllText($manifestPath,$manifestOriginal)
foreach($policy in @('Compatible','MachineLearning')) {
 $lines=Set-PackageIniValues ($configOriginal -split '\r?\n') @{'FSR/ProviderPolicy'=$policy}
 [IO.File]::WriteAllLines($config,$lines);Update-ManifestFile 'SKSE/Plugins/RaZkolbaS.ini'
 & $validate -Edition $Edition -PackageDirectory $package -FrameGeneration
 Require ((Read-PackageIni $config)['FSR/ProviderPolicy'] -eq $policy) ("FgIndependentSrProvider-"+$policy)
}
foreach($change in @(@('Settings/NativeUI','false'),@('Experimental/NativeUICompositionMode','1'),@('FSR/ProviderPolicy','Invalid'),@('Upscaling Advanced/FsrOrdinaryPresenter','true'))) {
 if($change[0] -eq 'FSR/ProviderPolicy'){
  # The writer refuses invalid enums; corrupt the public fixture directly.
  $lines=@($configOriginal -split '\r?\n' | ForEach-Object {
   if($_ -match '^\s*Provider\s*='){'Provider = Invalid'}else{$_}
  })
 }else{$lines=Set-PackageIniValues ($configOriginal -split '\r?\n') @{$change[0]=$change[1]}}
 [IO.File]::WriteAllLines($config,$lines)
 if($change[0] -eq 'FSR/ProviderPolicy'){
  Require ((Read-PackageIni $config -Raw)['FSR/Provider'] -eq 'Invalid') 'Invalid public provider fixture applied'
 }elseif($change[0] -eq 'Upscaling Advanced/FsrOrdinaryPresenter'){
  Require ((Read-PackageIni $config -Raw)['Upscaling Advanced/FsrOrdinaryPresenter'] -eq 'true') 'Invalid ordinary presenter fixture applied'
 }else{Require ((Read-PackageIni $config)[$change[0]] -eq $change[1]) ("ConfigMutationApplied-"+$change[0])}
 Update-ManifestFile 'SKSE/Plugins/RaZkolbaS.ini'
 Rejected {& $validate -Edition $Edition -PackageDirectory $package -FrameGeneration} ("InvalidFgConfig-"+$change[0])
}
[IO.File]::WriteAllText($config,$configOriginal);[IO.File]::WriteAllText($manifestPath,$manifestOriginal)
Rejected {& $stage -Edition $Edition -BuildDirectory $BuildDirectory -RuntimeDirectory $RuntimeDirectory -OutputDirectory $root -FrameGeneration} 'StagingNeverOverwritesPackage'
$fakeBuild=Join-Path $root 'fg-off-build';[IO.Directory]::CreateDirectory((Join-Path $fakeBuild 'Release'))|Out-Null
Copy-Item -LiteralPath (Join-Path $BuildDirectory 'Release/RaZkolbaS.dll') -Destination (Join-Path $fakeBuild 'Release/RaZkolbaS.dll')
$cache=[IO.File]::ReadAllText((Join-Path $BuildDirectory 'CMakeCache.txt'));[IO.File]::WriteAllText((Join-Path $fakeBuild 'CMakeCache.txt'),$cache.Replace('TRP_ENABLE_FSR_FG:BOOL=ON','TRP_ENABLE_FSR_FG:BOOL=OFF'))
Rejected {& $stage -Edition $Edition -BuildDirectory $fakeBuild -RuntimeDirectory $RuntimeDirectory -OutputDirectory (Join-Path $root 'bad-build') -FrameGeneration} 'FgBuildCapabilityRequired'
$srOnly=Join-Path $root 'sr-sdk/runtime';[IO.Directory]::CreateDirectory($srOnly)|Out-Null
$pin=Get-Content -LiteralPath (Join-Path $repository 'tools/fsr/runtime-pin.json') -Raw|ConvertFrom-Json
foreach($file in $pin.runtime){Copy-Item -LiteralPath (Join-Path $RuntimeDirectory $file.filename) -Destination $srOnly}
foreach($header in $pin.headers){$target=Join-Path (Split-Path $srOnly) $header.path;[IO.Directory]::CreateDirectory((Split-Path $target))|Out-Null;Copy-Item -LiteralPath (Join-Path (Split-Path $RuntimeDirectory) $header.path) -Destination $target}
& $stage -Edition $Edition -BuildDirectory $BuildDirectory -RuntimeDirectory $srOnly -OutputDirectory (Join-Path $root 'sr-package')
$sr=Join-Path $root ('sr-package/'+$Edition+'-FSR-SR');& $validate -Edition $Edition -PackageDirectory $sr
Require (@(Get-ChildItem -LiteralPath (Join-Path $sr 'SKSE/Plugins/FSR') -Filter '*.dll').Count -eq 2) 'SrPackageStillHasTwoModulesAndNeedsNoFgHeaders'
Require ((Get-FileHash -LiteralPath $defaultIni).Hash -eq $defaultHash) 'DefaultNvidiaPackageUnchanged'
Write-Output 'PASS: FG packaging admission, runtime pin, native UI configuration and SR-only compatibility'
