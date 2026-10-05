$ErrorActionPreference='Stop'
. (Join-Path $PSScriptRoot '../ini/IniLayoutCommon.ps1')
function Assert-PinnedFile([string]$Path,[string]$Hash,[long]$Bytes=-1) {
    if(-not (Test-Path -LiteralPath $Path -PathType Leaf)){throw "Required file absent: $Path"}
    if($Bytes -ge 0 -and (Get-Item -LiteralPath $Path).Length -ne $Bytes){throw "Pinned size mismatch: $Path"}
    if((Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant() -ne $Hash){throw "SHA-256 mismatch: $Path"}
}
function Get-PEImports([string]$Path) {
    $data=[IO.File]::ReadAllBytes($Path)
    if($data.Length -lt 256 -or [BitConverter]::ToUInt16($data,0) -ne 0x5a4d){throw "Invalid PE: $Path"}
    $pe=[BitConverter]::ToInt32($data,0x3c)
    if($pe -lt 64 -or $pe+264 -gt $data.Length -or [BitConverter]::ToUInt32($data,$pe) -ne 0x4550 -or [BitConverter]::ToUInt16($data,$pe+4) -ne 0x8664){throw "Not an x64 PE: $Path"}
    $optional=$pe+24
    if([BitConverter]::ToUInt16($data,$optional) -ne 0x20b){throw "Not PE32+: $Path"}
    $sections=[BitConverter]::ToUInt16($data,$pe+6);$table=$optional+[BitConverter]::ToUInt16($data,$pe+20)
    if($table+$sections*40 -gt $data.Length){throw "Invalid sections: $Path"}
    $headerSize=[BitConverter]::ToUInt32($data,$optional+60)
    function RvaOffset([long]$rva) {
        if($rva -lt $headerSize -and $rva -lt $data.Length){return [int]$rva}
        for($i=0;$i -lt $sections;$i++) {
            $s=$table+$i*40;$start=[BitConverter]::ToUInt32($data,$s+12);$size=[BitConverter]::ToUInt32($data,$s+16);$raw=[BitConverter]::ToUInt32($data,$s+20)
            if($rva -ge $start -and $rva-$start -lt $size -and $raw+$rva-$start -lt $data.Length){return [int]($raw+$rva-$start)}
        }
        throw "Import RVA outside PE: $Path"
    }
    foreach($directory in @(1,13)) {
        $rva=[BitConverter]::ToUInt32($data,$optional+112+$directory*8)
        if($rva -eq 0){continue}
        $offset=RvaOffset $rva;$stride=if($directory -eq 1){20}else{32}
        for($entry=0;$entry -lt 4096;$entry++) {
            $position=$offset+$entry*$stride
            if($position+$stride -gt $data.Length){throw "Truncated import table: $Path"}
            $name=[BitConverter]::ToUInt32($data,$position+$(if($directory -eq 1){12}else{4}))
            if($name -eq 0){break}
            if($directory -eq 13 -and ([BitConverter]::ToUInt32($data,$position) -band 1) -eq 0){throw "Unsupported VA-based delayed import table: $Path"}
            $nameOffset=RvaOffset $name;$end=$nameOffset
            while($end -lt $data.Length -and $end-$nameOffset -lt 4096 -and $data[$end] -ne 0){$end++}
            if($end -eq $data.Length -or $end-$nameOffset -ge 4096){throw "Invalid import name: $Path"}
            $module=[Text.Encoding]::ASCII.GetString($data,$nameOffset,$end-$nameOffset)
            [pscustomobject]@{module=$module;kind=$(if($directory -eq 1){'normal'}else{'delayed'})}
        }
        if($entry -ge 4096){throw "Unbounded import table: $Path"}
    }
}
function Read-PackageIni([string]$Path,[switch]$Raw) {
    $settings=@{};$section=''
    foreach($line in [IO.File]::ReadAllLines($Path)) {
        $line=$line.Trim();if(-not $line -or $line.StartsWith(';') -or $line.StartsWith('#')){continue}
        if($line -match '^\[([^\]]+)\]$'){$section=$Matches[1];continue}
        if($line -notmatch '^([^=]+)=(.*)$'){throw "Invalid INI line: $line"}
        $key=$section+'/'+$Matches[1].Trim();if($settings.ContainsKey($key)){throw "Duplicate INI key: $key"};$settings[$key]=$Matches[2].Trim()
    }
    if($Raw){return $settings}
    return ConvertTo-IniReadView $settings
}

function Assert-NoVendorImports([string]$Path) {
    foreach($import in @(Get-PEImports $Path)) {
        if($import.module -match '^(sl\.|_?nvngx|nvapi|amd_fidelityfx)'){throw "Mandatory vendor import $($import.module) in $Path"}
    }
}
function Get-EmbeddedBuildIdentity([string]$Path) {
    $text=[Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($Path))
    $matches=[regex]::Matches($text,'TRP_BUILD\|edition=(Standard|Universal)\|source=([a-z0-9-]+)\|FSR=([01])\|FG=([01])\|NR=([01])')
    if($matches.Count -ne 1){throw "Missing or ambiguous embedded build identity: $Path"}
    $g=$matches[0].Groups
    [pscustomobject]@{edition=$g[1].Value;sourceRevision=$g[2].Value;sourceClean=($g[2].Value -match '^[0-9a-f]{12}$');fsrCompiled=($g[3].Value -eq '1');frameGenerationCompiled=($g[4].Value -eq '1');neuralRenderingCompiled=($g[5].Value -eq '1')}
}
