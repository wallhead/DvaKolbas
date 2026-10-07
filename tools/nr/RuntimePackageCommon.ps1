# Logical hardware profiles may share one physical runtime. Validate/copy it once.
. (Join-Path $PSScriptRoot '../fsr/PackageCommon.ps1')
function Test-PostSrTrialRoute([hashtable]$Ini) {
    $mode=$Ini['Settings/UpscaleType'];$presenter=$Ini['Experimental/FrameGenerationBackend']
    if($mode -eq '3') { return $presenter -eq '1' }
    if($mode -eq '0') {
        return $presenter -eq '1' -and (-not $Ini.ContainsKey('Settings/QualityLevel') -or
            $Ini['Settings/QualityLevel'] -in @('0','1','2','3','4'))
    }
    return $mode -eq '4' -and $presenter -in @('0','2') -and
        $Ini['FSR/Quality'] -in @('Quality','Balanced','Performance','NativeAA') -and
        $Ini['FSR/SourceColorEncoding'] -eq $Ini['NeuralRendering/SourceColorEncoding']
}
function ConvertTo-PortableNrPackageIni([string[]]$Lines) {
    # Packaged NR models are relative to the virtual Data tree. Keep research
    # core paths in validation receipts, never in another user's startup INI.
    Set-PackageIniValues $Lines @{
        'Runtime/NRRuntimeRoot'='';'Runtime/NRDriverCore'='';
        'NeuralRendering/RuntimeRoot'='';'NeuralRendering/DriverCore'=''
    }
}
function Assert-PortableNrPackageIni([hashtable]$Ini) {
    foreach($key in @('Runtime/NRRuntimeRoot','Runtime/NRDriverCore')){
        if(-not $Ini.ContainsKey($key)){throw "Packaged NR path key is missing: $key"}
        if($Ini[$key]){throw "Packaged NR path is not portable: $key"}
    }
    foreach($alias in Get-IniLayoutAliases){
        if($Ini.ContainsKey($alias.Legacy)){throw "Obsolete packaged INI key: $($alias.Legacy)"}
    }
}
function Read-PortableNrPackageIni([string]$Path) {
    # Require the current layout before constructing the internal reader view.
    $ini=Read-PackageIni $Path -Raw
    Assert-PortableNrPackageIni $ini
    return ConvertTo-IniReadView $ini
}
function Write-PortableModMetadata([string]$Directory,[string]$Revision) {
    if($Revision -notmatch '^[a-zA-Z0-9._-]+$'){throw 'Invalid metadata revision'}
    $lines=@('[General]','gameName=SkyrimSE','modid=0','version=1.1','newestVersion=','category=0','nexusFileStatus=1',
        'installationFile=RaZKolbaS DLSS FSR FG NR v1.1.zip','repository=','ignoredVersion=',
        'comments=RaZKolbaS DLSS FSR FG NR',"notes=Release v1.1; Build $Revision; DLSS/FSR -> NR -> FG -> UI.",
        'url=https://github.com/wallhead/RaZkolbaS','hasCustomURL=true','converted=false','validated=false','',
        '[installedFiles]','size=0')
    [IO.File]::WriteAllLines((Join-Path $Directory 'meta.ini'),$lines,[Text.UTF8Encoding]::new($false))
}
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
