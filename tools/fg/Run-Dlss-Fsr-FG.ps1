param(
    [string]$BuildDirectory = (Join-Path $PSScriptRoot '../../out/build/fsr-fg-universal'),
    [string]$OutputDirectory
)
$ErrorActionPreference = 'Stop'
$build = (Resolve-Path -LiteralPath $BuildDirectory).Path
if (!$OutputDirectory) { $OutputDirectory = Join-Path $build 'validation/mixed-route' }
if (Test-Path -LiteralPath $OutputDirectory) {
    throw 'Use a fresh output directory; stale receipts must not qualify this run.'
}
$cache = Get-Content -LiteralPath (Join-Path $build 'CMakeCache.txt')
function CachePath([string]$Name) {
    $line = @($cache | Where-Object { $_ -match ('^' + [regex]::Escape($Name) + ':[^=]+=' ) })
    if ($line.Count -ne 1) { throw "Missing cached payload: $Name" }
    return ($line[0] -split '=', 2)[1]
}
$fixture = Join-Path $build 'tests/fsr-fg/Release/TRPDlssFsrGenerationGpuTests.exe'
$nr = CachePath 'TRP_NR_RTX40_DLL'
$core = CachePath 'TRP_NR_DRIVER_CORE'
$runtime = Join-Path (CachePath 'TRP_FSR_SDK_DIR') 'runtime'
foreach ($path in @($fixture,$nr,$core,$runtime)) {
    if (!(Test-Path -LiteralPath $path)) { throw "NOT QUALIFIED: missing $path" }
}
if (Get-Process SkyrimSE -ErrorAction SilentlyContinue) { throw 'Keep Skyrim closed for GPU checks.' }
New-Item -ItemType Directory -Path $OutputDirectory | Out-Null
foreach ($quality in @('native','quality','performance','negative-control')) {
    $report = Join-Path $OutputDirectory ($quality + '.json')
    $arguments = @('--quality',$(if ($quality -eq 'negative-control') {'native'} else {$quality}),
        '--nr',$nr,'--core',$core,'--runtime',$runtime,'--output',$report)
    if ($quality -eq 'negative-control') { $arguments += '--omit-nr' }
    & $fixture @arguments
    $code = $LASTEXITCODE
    if (!(Test-Path -LiteralPath $report)) { throw "NOT QUALIFIED: $quality produced no report" }
    $receipt = Get-Content -Raw -LiteralPath $report | ConvertFrom-Json
    if ($receipt.hashes.executable -ne (Get-FileHash -LiteralPath $fixture -Algorithm SHA256).Hash.ToLowerInvariant()) {
        throw 'NOT QUALIFIED: receipt/executable mismatch'
    }
    if ($quality -eq 'negative-control') {
        if ($code -eq 0 -or $receipt.qualified -or $receipt.nrDispatches -ne 0 -or
            $receipt.failure -ne 'no actual NR enhancement observed') { throw 'NR negative control did not fail as intended.' }
    } elseif ($code -ne 0 -or !$receipt.qualified -or $receipt.dlssDispatches -ne 160 -or
        $receipt.nrDispatches -eq 0 -or $receipt.fsrSrCreates -ne 0 -or $receipt.fsrSrDispatches -ne 0 -or
        $receipt.generatedCallbacks -le 20 -or $receipt.generatedPixelReadbacks -le 20 -or
        $receipt.uiFailures -ne 0 -or $receipt.retirementFailures -ne 0 -or
        $receipt.pendingReaderRetirements -ne 2 -or $receipt.resetReentries -ne $(if ($quality -eq 'negative-control') {12} else {14})) {
        throw "NOT QUALIFIED: $quality failed the mixed-route evidence contract"
    }
}
Write-Output "PASS: Native, Quality, Performance and NR negative control. Receipts: $OutputDirectory"

$global:LASTEXITCODE = 0
