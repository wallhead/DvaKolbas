param(
    [Parameter(Mandatory=$true)][string]$RuntimeRoot,
    [Parameter(Mandatory=$true)][string]$DriverCore,
    [Parameter(Mandatory=$true)][string]$NgxInclude,
    [string]$CMake='C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe',
    [string]$OutputRoot='out/research/nr-identity',
    [switch]$ResourceIsolationOnly,
    [switch]$DeferredCapture
)
$ErrorActionPreference='Stop'
$OutputRoot=[IO.Path]::GetFullPath($OutputRoot)
$RuntimeRoot=[IO.Path]::GetFullPath($RuntimeRoot)
$DriverCore=[IO.Path]::GetFullPath($DriverCore)
[IO.Directory]::CreateDirectory($OutputRoot)|Out-Null
$build=Join-Path $OutputRoot 'build'
$variants=@(
    @{Name='rotating';Single='OFF';Creation='OFF'},
    @{Name='fixed-parameters';Single='OFF';Creation='ON'},
    @{Name='fixed-resources';Single='ON';Creation='OFF'},
    @{Name='fixed-both';Single='ON';Creation='ON'}
)
if($ResourceIsolationOnly){$variants=@(@{Name='fixed-io-rotating-params';Single='OFF';Creation='OFF'})}
if($DeferredCapture -and -not $ResourceIsolationOnly){$variants+=@{Name='drain-only';Single='OFF';Creation='OFF'}}
$cases=@(
    @{Name='context-sdr-1';Scene='context';Domain='sdr';Step=2;Passes=1},
    @{Name='pan2-sdr-1';Scene='pan';Domain='sdr';Step=2;Passes=1},
    @{Name='pan16-sdr-1';Scene='pan';Domain='sdr';Step=16;Passes=1},
    @{Name='pan64-sdr-1';Scene='pan';Domain='sdr';Step=64;Passes=1},
    @{Name='context-sdr-3';Scene='context';Domain='sdr';Step=2;Passes=3},
    @{Name='pan16-sdr-3';Scene='pan';Domain='sdr';Step=16;Passes=3},
    @{Name='context-linear-1';Scene='context';Domain='linear';Step=2;Passes=1}
)
$runs=@()
foreach($variant in $variants){
    if(Get-Process SkyrimSE -ErrorAction SilentlyContinue){throw 'Skyrim is running; standalone work deferred'}
    $fixedIO=if($variant.Name -eq 'fixed-io-rotating-params'){'ON'}else{'OFF'}
    $drain=if($variant.Name -eq 'drain-only'){'ON'}else{'OFF'}
    & $CMake -S $PSScriptRoot -B $build -G 'Visual Studio 17 2022' -A x64 "-DNR_NGX_INCLUDE=$NgxInclude" "-DNR_IDENTITY_SINGLE_SLOT=$($variant.Single)" "-DNR_IDENTITY_CREATION_PARAMETERS=$($variant.Creation)" "-DNR_IDENTITY_FIXED_IO_ROTATING_PARAMETERS=$fixedIO" "-DNR_IDENTITY_DRAIN_EACH_SOURCE=$drain" *> (Join-Path $OutputRoot "configure-$($variant.Name).log")
    if($LASTEXITCODE -ne 0){throw "Configure failed: $($variant.Name)"}
    & $CMake --build $build --config Release --target NrIdentityProbe --parallel 2 *> (Join-Path $OutputRoot "build-$($variant.Name).log")
    if($LASTEXITCODE -ne 0){throw "Build failed: $($variant.Name)"}
    $executable=Join-Path $OutputRoot "NrIdentityProbe-$($variant.Name).exe"
    Copy-Item -LiteralPath (Join-Path $build 'Release/NrIdentityProbe.exe') -Destination $executable
    $exeHash=(Get-FileHash -LiteralPath $executable).Hash.ToLowerInvariant()
    $stageHash=(Get-FileHash -LiteralPath (Join-Path $build 'IdentityStage.cpp')).Hash.ToLowerInvariant()
    $bridgeHash=(Get-FileHash -LiteralPath (Join-Path $build 'IdentityBridge.cpp')).Hash.ToLowerInvariant()
    foreach($case in $cases){
        $directory=Join-Path $OutputRoot "$($variant.Name)/$($case.Name)"
        [IO.Directory]::CreateDirectory($directory)|Out-Null
        $probeArguments=@($RuntimeRoot,$DriverCore,$directory,'correct',$case.Step,$case.Passes,$case.Scene,$case.Domain)
        if($DeferredCapture){$probeArguments+='deferred'}
        & $executable @probeArguments *> (Join-Path $directory 'run.log')
        if($LASTEXITCODE -ne 0){throw "Probe failed: $($variant.Name)/$($case.Name)"}
        $frames=@(Import-Csv (Join-Path $directory 'frames.csv'))
        $expected=if($case.Scene -eq 'context'){160}else{120}
        if($frames.Count -ne $expected -or @($frames | Where-Object {$_.nrEvaluated -ne '1' -or $_.outputSha256 -notmatch '^[0-9a-f]{64}$'}).Count){throw 'Invalid frame receipt'}
        $runs+=@{variant=$variant.Name;case=$case.Name;directory=$directory;sources=$frames.Count;passes=$case.Passes;scene=$case.Scene;domain=$case.Domain;panStep=$case.Step;executableSha256=$exeHash;stageSha256=$stageHash;bridgeSha256=$bridgeHash;csvSha256=(Get-FileHash -LiteralPath (Join-Path $directory 'frames.csv')).Hash.ToLowerInvariant()}
        Write-Output "Recorded $($variant.Name)/$($case.Name): $expected real sources"
    }
}
# Controls use the unchanged rotating variant. No vendor evaluations allowed.
if(-not $ResourceIsolationOnly){foreach($scene in @('pan','context')){
    $directory=Join-Path $OutputRoot "off-$scene"
    [IO.Directory]::CreateDirectory($directory)|Out-Null
    $probeArguments=@($RuntimeRoot,$DriverCore,$directory,'off',16,1,$scene,'sdr')
    if($DeferredCapture){$probeArguments+='deferred'}
    & (Join-Path $OutputRoot 'NrIdentityProbe-rotating.exe') @probeArguments *> (Join-Path $directory 'run.log')
    if($LASTEXITCODE -ne 0){throw 'Off control failed'}
    $frames=@(Import-Csv (Join-Path $directory 'frames.csv'))
    if(@($frames | Where-Object {$_.nrEvaluated -ne '0' -or [double]$_.meanRgbCorrection -ne 0}).Count){throw 'Off control modified source'}
    Write-Output "Recorded off-${scene}: $($frames.Count) unchanged sources"
}}
$receiptName=if($ResourceIsolationOnly){'runs-io-isolation.json'}else{'runs.json'}
@{schema=1;runtimeRoot=$RuntimeRoot;driverCore=$DriverCore;driverCoreSha256=(Get-FileHash -LiteralPath $DriverCore).Hash.ToLowerInvariant();runs=$runs;capture=if($DeferredCapture){'Deferred per-source copies, Map after dispatch'}else{'Serial Map every source'};scope='Native SDR, constant depth, no jitter/SR/FG/gameplay';productChanged=$false} | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath (Join-Path $OutputRoot $receiptName) -Encoding utf8
