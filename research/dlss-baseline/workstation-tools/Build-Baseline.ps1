param([ValidateSet('standard','universal')][string]$Edition = 'standard')
$ErrorActionPreference = 'Stop'
$workspace = Split-Path $PSScriptRoot -Parent
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
$vsInstallation = & $vswhere -version '[17.0,18.0)' -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $vsInstallation) { throw 'Visual Studio 2022 C++ Build Tools are required.' }
$cmakeDirectory = Join-Path $vsInstallation 'Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin'
$cmake = Join-Path $cmakeDirectory 'cmake.exe'
$ctest = Join-Path $cmakeDirectory 'ctest.exe'
Push-Location $workspace
try {
    New-Item -ItemType Directory -Path 'out/validation' -Force | Out-Null
    & $cmake --preset "baseline-$Edition" 2>&1 | Tee-Object -FilePath "out/validation/configure-$Edition.log"
    if ($LASTEXITCODE) { throw "Configure failed: $LASTEXITCODE" }
    & $cmake --build "out/build/$Edition" --config Release --parallel 2 2>&1 | Tee-Object -FilePath "out/validation/build-$Edition.log"
    if ($LASTEXITCODE) { throw "Build failed: $LASTEXITCODE" }
    & $ctest --test-dir "out/build/$Edition" -C Release --output-on-failure --no-tests=error 2>&1 | Tee-Object -FilePath "out/validation/tests-$Edition.log"
    if ($LASTEXITCODE) { throw "CTest failed: $LASTEXITCODE" }
} finally { Pop-Location }
