param([Parameter(Mandatory)][string]$SdkDirectory,[Parameter(Mandatory)][string]$ScratchRoot,[Parameter(Mandatory)][string]$CMake,[string]$Repository)
$ErrorActionPreference='Stop'
if(-not $Repository){$Repository=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))}
$sdk=[IO.Path]::GetFullPath($SdkDirectory)
if(Test-Path -LiteralPath (Join-Path $sdk 'Kits/FidelityFX/framegeneration')){throw 'This check requires a minimal SR SDK without any FG headers'}
if(Test-Path -LiteralPath (Join-Path $sdk 'runtime/amd_fidelityfx_framegeneration_dx12.dll')){throw 'This check requires no FG runtime'}
$root=Join-Path ([IO.Path]::GetFullPath($ScratchRoot)) ([guid]::NewGuid().ToString('N'))
$source=Join-Path $root 'source';[IO.Directory]::CreateDirectory($source)|Out-Null
# Exercise the exact production CMake boundary and complete SR library, without game/vcpkg dependencies.
Copy-Item -LiteralPath (Join-Path $Repository 'src') -Destination (Join-Path $source 'src') -Recurse
foreach($folder in @('cmake','tools/fsr')){[IO.Directory]::CreateDirectory((Join-Path $source $folder))|Out-Null}
Copy-Item -LiteralPath (Join-Path $Repository 'cmake/FSR.cmake') -Destination (Join-Path $source 'cmake/FSR.cmake')
foreach($pin in @('runtime-pin.json','fg-runtime-pin.json')){Copy-Item -LiteralPath (Join-Path $Repository ('tools/fsr/'+$pin)) -Destination (Join-Path $source ('tools/fsr/'+$pin))}
@'
cmake_minimum_required(VERSION 3.21)
project(FsrHeaderIsolation LANGUAGES CXX)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/stub.cpp" "void package_header_probe() {}")
add_library(TheosRenderPipeline STATIC "${CMAKE_CURRENT_BINARY_DIR}/stub.cpp")
include(cmake/FSR.cmake)
'@ | Set-Content -LiteralPath (Join-Path $source 'CMakeLists.txt')
$bad=Join-Path $root 'fg-on';$good=Join-Path $root 'fg-off'
& $CMake -S $source -B $bad -G 'Visual Studio 17 2022' -A x64 '-DTRP_ENABLE_FSR=ON' '-DTRP_ENABLE_FSR_FG=ON' "-DTRP_FSR_SDK_DIR=$sdk" *> (Join-Path $root 'fg-on.log')
if($LASTEXITCODE -eq 0 -or (Get-Content (Join-Path $root 'fg-on.log') -Raw) -notmatch 'Pinned FG header missing'){throw 'FG-on did not reject missing FG headers for the expected reason'}
Write-Output 'PASS: FgOnRequiresHeaders'
& $CMake -S $source -B $good -G 'Visual Studio 17 2022' -A x64 '-DTRP_ENABLE_FSR=ON' '-DTRP_ENABLE_FSR_FG=OFF' "-DTRP_FSR_SDK_DIR=$sdk" *> (Join-Path $root 'fg-off-configure.log')
if($LASTEXITCODE -ne 0){Get-Content (Join-Path $root 'fg-off-configure.log') -Tail 15;throw 'FG-off configure failed'}
& $CMake --build $good --config Release --target TRPFsrRuntime --parallel 2 *> (Join-Path $root 'fg-off-build.log')
if($LASTEXITCODE -ne 0){Get-Content (Join-Path $root 'fg-off-build.log') -Tail 15;throw 'FG-off SR library failed to compile'}
if(-not (Test-Path -LiteralPath (Join-Path $good 'Release/TRPFsrRuntime.lib'))){throw 'Missing compiled SR artifact'}
Write-Output "PASS: FgOffBuildNeedsNoHeaders; exact production SR sources compiled with no FG headers/runtime: $root"
