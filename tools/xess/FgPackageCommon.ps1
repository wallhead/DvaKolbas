. (Join-Path $PSScriptRoot '../fsr/PackageCommon.ps1')
function Assert-XessGenerationBuild($Identity) {
    if(-not $Identity.xessFrameGenerationCompiled -or -not $Identity.fsrCompiled -or -not $Identity.sourceClean){throw 'Intel FG trial requires its independent compiled capability, FSR source support and clean revision'}
}
function Assert-XessGenerationPayload([string]$Root,$Pin) {
    foreach($file in $Pin.generationRuntimes){
        Assert-PinnedFile (Join-Path ([IO.Path]::GetFullPath($Root)) $file.stagePath) $file.sha256 $file.bytes
    }
}
function Assert-XessGenerationIni([string]$Path) {
    $raw=Read-PackageIni $Path -Raw;$converted=Read-PackageIni $Path
    foreach($item in @{'Upscaling/Upscaler'='FSR';'FSR/Quality'='Native';'FSR/Provider'='FSR3';'FSR/SourceColorEncoding'='Gamma22';'FrameGeneration/Backend'='XeSS';'FrameGeneration/Enabled'='false';'NeuralRendering/Enabled'='false';'HDROutput/Enabled'='false';'DynamicResolution/Enabled'='false';'DynamicResolution/Oscillate'='false';'Interface/NativeUI'='true';'FrameGeneration/UIComposition'='Dedicated'}.GetEnumerator()){
        if($raw[$item.Key] -cne $item.Value){throw "Intel initial trial setting mismatch: $($item.Key)"}
    }
    if($converted['FrameGeneration/Backend'] -cne '3' -or $converted['Settings/UpscaleType'] -cne '4'){throw 'Intel Native trial does not decode to the requested source/presenter'}
}
function Assert-XessValidationReceipt([string]$Path,[string]$Plugin) {
    if(-not $Path){throw 'Intel trial requires a validated build receipt'}
    $receipt=Get-Content -LiteralPath $Path -Raw | ConvertFrom-Json
    $identity=Get-EmbeddedBuildIdentity $Plugin;Assert-XessGenerationBuild $identity
    if($receipt.schema -ne 1 -or $receipt.allChecksPassed -ne $true -or $receipt.sourceRevision -cne $identity.sourceRevision -or
        $receipt.pluginSha256 -cne (Get-FileHash -LiteralPath $Plugin -Algorithm SHA256).Hash.ToLowerInvariant()){
        throw 'Intel trial validation receipt differs from this clean compiled plugin'
    }
}
