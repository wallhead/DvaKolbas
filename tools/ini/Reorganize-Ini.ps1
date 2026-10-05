param(
    [Parameter(Mandatory)][string]$SourceIni,
    [Parameter(Mandatory)][string]$OutputIni,
    [string]$TemplateIni=(Join-Path $PSScriptRoot '../../package/SKSE/Plugins/TheosRenderPipeline.ini')
)
# Writes a separate reviewable file. Never changes the source or fills missing
# optional settings from the template: absent values must retain reader defaults.
. (Join-Path $PSScriptRoot '../fsr/PackageCommon.ps1')
$source=(Resolve-Path -LiteralPath $SourceIni).Path
$output=[IO.Path]::GetFullPath($OutputIni)
if($source -eq $output -or (Test-Path -LiteralPath $output)){throw 'Use a new output file; source INI stays untouched'}
$sourceHash=(Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash
$values=Read-PackageIni $source -Raw
foreach($retired in @('SourceDLSSG/NRStableColors','NeuralRendering/StableColors')){
    $values.Remove($retired)
}
foreach($alias in Get-IniLayoutAliases){
    if($values.ContainsKey($alias.Legacy)){
        if(-not $values.ContainsKey($alias.Canonical)){$values[$alias.Canonical]=$values[$alias.Legacy]}
        $values.Remove($alias.Legacy)
    }
}
$values['Settings/ConfigVersion']='2'
$sections=[ordered]@{};$section='';$comments=[Collections.Generic.List[string]]::new()
# Template controls order and comments, but never supplies new values.
foreach($line in [IO.File]::ReadAllLines($TemplateIni)){
    if($line -match '^\[([^\]]+)\]$'){
        $section=$Matches[1];if(-not $sections.Contains($section)){$sections[$section]=[Collections.Generic.List[string]]::new()}
        $comments.Clear();continue
    }
    if($line.TrimStart().StartsWith(';')){$comments.Add($line);continue}
    if($line -match '^\s*([^=]+)=(.*)$'){
        $name=$Matches[1].Trim();$key=$section+'/'+$name
        if($values.ContainsKey($key)){
            $sections[$section].AddRange([string[]]$comments.ToArray())
            $sections[$section].Add($name+' = '+$values[$key]);$values.Remove($key)
        }
        $comments.Clear()
    }
}
# Keep extra profile/unknown keys and their user comments.
$unknownComments=@{};$comments.Clear();$section=''
foreach($line in [IO.File]::ReadAllLines($source)){
    if($line -match '^\[([^\]]+)\]$'){$section=$Matches[1];$comments.Clear();continue}
    if($line.TrimStart() -match '^[;#]'){$comments.Add($line);continue}
    if($line -match '^\s*([^=]+)=(.*)$'){
        $key=$section+'/'+$Matches[1].Trim();$unknownComments[$key]=[string[]]$comments.ToArray();$comments.Clear()
    }
}
foreach($key in @($values.Keys | Sort-Object)){
    $parts=$key.Split('/',2);$section=$parts[0]
    if(-not $sections.Contains($section)){$sections[$section]=[Collections.Generic.List[string]]::new()}
    if($unknownComments.ContainsKey($key)){$sections[$section].AddRange([string[]]$unknownComments[$key])}
    $sections[$section].Add($parts[1]+' = '+$values[$key])
}
$lines=[Collections.Generic.List[string]]::new()
$lines.Add('; DvaKolbas settings. Existing values preserved; layout version 2.')
$lines.Add('; Backend, quality and runtime paths require a restart. Live controls use Apply.')
foreach($entry in $sections.GetEnumerator()){
    if(-not $entry.Value.Count){continue}
    $lines.Add('');$lines.Add('['+$entry.Key+']');$lines.AddRange([string[]]$entry.Value.ToArray())
}
# Parse and compare before publishing the result. Only names/layout may change.
$temporary=$output+'.tmp-'+[Guid]::NewGuid().ToString('N')
try {
    [IO.File]::WriteAllLines($temporary,$lines,[Text.UTF8Encoding]::new($false))
    $before=Read-PackageIni $source;$after=Read-PackageIni $temporary
foreach($key in $before.Keys){
    if($key -in @('SourceDLSSG/NRStableColors','NeuralRendering/StableColors')){continue}
        if($key -eq 'Settings/ConfigVersion'){continue}
        if(-not $after.ContainsKey($key) -or $after[$key] -cne $before[$key]){throw "INI value changed: $key"}
    }
    if((Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash -ne $sourceHash){throw 'Source INI changed during conversion'}
    Move-Item -LiteralPath $temporary -Destination $output
} finally {
    if(Test-Path -LiteralPath $temporary){Remove-Item -LiteralPath $temporary}
}
Write-Output "REORGANIZED ONLY: $output"
