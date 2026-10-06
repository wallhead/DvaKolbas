$ErrorActionPreference='Stop'
. (Join-Path $PSScriptRoot '../tools/nr/RuntimePackageCommon.ps1')
foreach($mode in @('0','3','4')) {
    foreach($quality in @('Quality','Balanced','Performance','NativeAA')) {
        $route=@{'Settings/UpscaleType'=$mode;'Experimental/FrameGenerationBackend'=$(if($mode -eq '4'){'2'}else{'1'});'FSR/Quality'=$quality;'FSR/SourceColorEncoding'='Gamma22';'NeuralRendering/SourceColorEncoding'='Gamma22'}
        if(-not (Test-PostSrTrialRoute $route)){throw "Supported fixed After route rejected: $mode/$quality"}
    }
}
$route['FSR/Quality']='Unknown'
if(Test-PostSrTrialRoute $route){throw 'Unknown FSR quality accepted'}
$route['FSR/Quality']='Quality';$route['FSR/SourceColorEncoding']='Linear'
if(Test-PostSrTrialRoute $route){throw 'Unqualified SDR route accepted'}
$route['Settings/UpscaleType']='0';$route['Experimental/FrameGenerationBackend']='2'
if(Test-PostSrTrialRoute $route){throw 'DLSS with FSR presenter accepted'}
$route['Experimental/FrameGenerationBackend']='1';$route['Settings/QualityLevel']='999'
if(Test-PostSrTrialRoute $route){throw 'Invalid fixed DLSS quality accepted'}
$lines=@('[Runtime]','NRRuntimeRoot = C:/Machine/NR','NRDriverCore = C:/Machine/Driver/_nvngx.dll','[NeuralRendering]','DriverCore = D:/Old/_nvngx.dll','RuntimeRoot = D:/Old/NR','[SourceDLSSG]','NRStyle = 2')
$converted=ConvertTo-PortableNrPackageIni $lines
$root=Join-Path ([IO.Path]::GetTempPath()) ('raz-portable-'+[guid]::NewGuid())
[IO.Directory]::CreateDirectory($root)|Out-Null
try {
    [IO.File]::WriteAllLines((Join-Path $root 'RaZkolbaS.ini'),[string[]]$converted)
    $ini=Read-PortableNrPackageIni (Join-Path $root 'RaZkolbaS.ini')
    if($ini['SourceDLSSG/NRStyle'] -ne '2'){throw 'Portable conversion changed the user style'}
    foreach($layout in @(
        @{Name='legacy';Lines=@('[NeuralRendering]','RuntimeRoot = D:/Old/NR','DriverCore = D:/Old/_nvngx.dll')},
        @{Name='current';Lines=@('[Runtime]','NRRuntimeRoot = D:/Current/NR','NRDriverCore = D:/Current/_nvngx.dll')}
    )){
        $path=Join-Path $root ($layout.Name+'.ini')
        [IO.File]::WriteAllLines($path,[string[]](ConvertTo-PortableNrPackageIni $layout.Lines))
        $null=Read-PortableNrPackageIni $path
    }
    foreach($bad in @(
        @{Name='conflict';Lines=@('[Runtime]','NRRuntimeRoot =','NRDriverCore =','[NeuralRendering]','RuntimeRoot = D:/Old/NR','DriverCore = D:/Old/_nvngx.dll')},
        @{Name='missing-core';Lines=@('[Runtime]','NRRuntimeRoot =')}
    )){
        $path=Join-Path $root ($bad.Name+'.ini');[IO.File]::WriteAllLines($path,[string[]]$bad.Lines)
        $rejected=$false
        try{$null=Read-PortableNrPackageIni $path}catch{$rejected=$true}
        if(-not $rejected){throw "Nonportable layout was accepted: $($bad.Name)"}
    }
    Write-PortableModMetadata -Directory $root -Revision 'test-revision'
    $metadata=[IO.File]::ReadAllText((Join-Path $root 'meta.ini'))
    if($metadata -match '(?im)^installationFile\s*=\s*[A-Za-z]:'){throw 'Metadata contains a machine-specific archive path'}
    if($metadata -notmatch 'installationFile=RaZKolbaS DLSS FSR FG NR v1\.0\.zip'){throw 'Archive basename missing'}
    if($metadata -notmatch '(?m)^version=1\.0\r?$'){throw 'Release version missing'}
    Write-Output 'PASS: portable package clears current/legacy NR paths, preserves settings, and writes portable MO2 metadata'
} finally {
    $full=[IO.Path]::GetFullPath($root)
    if(-not $full.StartsWith([IO.Path]::GetFullPath([IO.Path]::GetTempPath()),[StringComparison]::OrdinalIgnoreCase)){throw 'Unsafe cleanup'}
    Remove-Item -LiteralPath $full -Recurse -Force
}
