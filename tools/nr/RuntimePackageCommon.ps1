# Logical hardware profiles may share one physical runtime. Validate/copy it once.
. (Join-Path $PSScriptRoot '../fsr/PackageCommon.ps1')
function Get-NrPhysicalModels([object[]]$Profiles) {
    $paths=@{};$ids=@{}
    foreach($p in $Profiles){
        if(-not $p.id -or $ids.ContainsKey($p.id)){throw 'Missing or duplicate NR profile ID'}
        $ids[$p.id]=$true
        if($p.relativePath -notmatch '^NR/[^/\\:]+/nvngx_dlssnr\.dll$' -or
            $p.relativePath -match '(^|/)\.\.?(/|$)' -or
            $p.sha256 -notmatch '^[a-f0-9]{64}$' -or [long]$p.bytes -le 0){throw 'Invalid NR model pin/path'}
        if($paths.ContainsKey($p.relativePath)){
            $first=$paths[$p.relativePath]
            if($first.sha256 -ne $p.sha256 -or $first.bytes -ne $p.bytes -or
                $first.compatibility -ne $p.compatibility){throw 'Shared NR path has inconsistent identity/policy'}
        }else{$paths[$p.relativePath]=$p}
    }
    if(-not $paths.Count){throw 'NR catalog has no model payloads'}
    $paths.Values | Sort-Object relativePath
}
function Copy-NrRuntimeModels([object[]]$Profiles,[hashtable]$SourcesByPath,[string]$RuntimeRoot) {
    $physical=@(Get-NrPhysicalModels $Profiles)
    foreach($p in $physical){
        if(-not $SourcesByPath.ContainsKey($p.relativePath)){throw "NR source missing for $($p.relativePath)"}
        Assert-PinnedFile $SourcesByPath[$p.relativePath] $p.sha256 $p.bytes
    }
    foreach($p in $physical){
        $destination=Join-Path $RuntimeRoot $p.relativePath
        [IO.Directory]::CreateDirectory([IO.Path]::GetDirectoryName($destination))|Out-Null
        Copy-Item -LiteralPath $SourcesByPath[$p.relativePath] -Destination $destination
    }
}
function Assert-NrRuntimeModels([object[]]$Profiles,[string]$RuntimeRoot) {
    $physical=@(Get-NrPhysicalModels $Profiles);$expected=@{}
    foreach($p in $physical){
        $expected[$p.relativePath]=$true
        Assert-PinnedFile (Join-Path $RuntimeRoot $p.relativePath) $p.sha256 $p.bytes
    }
    foreach($file in Get-ChildItem -LiteralPath (Join-Path $RuntimeRoot 'NR') -Recurse -File){
        $relative=[IO.Path]::GetRelativePath([IO.Path]::GetFullPath($RuntimeRoot),$file.FullName).Replace('\','/')
        if(-not $expected.ContainsKey($relative)){throw "Unrequested NR payload: $relative"}
    }
}
function Copy-NrTrialFiles([object[]]$Profiles,[string]$SourceRoot,[string]$DestinationRoot) {
    $runtimeRelative='SKSE/Plugins/RaZkolbaS/'
    $sources=@{}
    foreach($p in @(Get-NrPhysicalModels $Profiles)){
        $sources[$p.relativePath]=Join-Path $SourceRoot ($runtimeRelative+$p.relativePath)
        Assert-PinnedFile $sources[$p.relativePath] $p.sha256 $p.bytes
    }
    # Snapshot before creating output. Skip every old NR payload and copy only pins.
    $files=@(Get-ChildItem -LiteralPath $SourceRoot -Recurse -File)
    foreach($file in $files){
        $relative=[IO.Path]::GetRelativePath([IO.Path]::GetFullPath($SourceRoot),$file.FullName).Replace('\','/')
        if($relative.StartsWith($runtimeRelative+'NR/',[StringComparison]::OrdinalIgnoreCase)){continue}
        $destination=Join-Path $DestinationRoot $relative
        [IO.Directory]::CreateDirectory([IO.Path]::GetDirectoryName($destination))|Out-Null
        Copy-Item -LiteralPath $file.FullName -Destination $destination
    }
    Copy-NrRuntimeModels $Profiles $sources (Join-Path $DestinationRoot $runtimeRelative)
}
