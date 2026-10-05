# Package tooling consumes the same migration table as the C++ settings reader.
function Get-IniLayoutAliases {
    $header=Join-Path $PSScriptRoot '../../src/IniLayout.h'
    $table=[regex]::Matches([IO.File]::ReadAllText($header),'Key\{"([^"]+)", "([^"]+)", "([^"]+)", "([^"]+)"\}')
    if(-not $table.Count){throw 'INI migration table is missing'}
    foreach($row in $table){
        [pscustomobject]@{Legacy=$row.Groups[1].Value+'/'+$row.Groups[2].Value;Canonical=$row.Groups[3].Value+'/'+$row.Groups[4].Value}
    }
}
function ConvertTo-IniReadView([hashtable]$Settings) {
    foreach($alias in Get-IniLayoutAliases){
        if($Settings.ContainsKey($alias.Canonical)){$Settings[$alias.Legacy]=$Settings[$alias.Canonical]}
    }
    return $Settings
}
function Get-IniCanonicalKey([string]$Key) {
    foreach($alias in Get-IniLayoutAliases){if($alias.Legacy -eq $Key){return $alias.Canonical}}
    return $Key
}
function Set-PackageIniValues([string[]]$Lines,[hashtable]$Values) {
    $wanted=@{};foreach($key in $Values.Keys){$wanted[(Get-IniCanonicalKey $key)]=$Values[$key]}
    $found=@{};$section='';$result=[Collections.Generic.List[string]]::new()
    foreach($line in $Lines){
        if($line.Trim() -match '^\[([^\]]+)\]$'){$section=$Matches[1]}
        elseif($line -match '^\s*([^=]+)=(.*)$' -and $line.TrimStart() -notmatch '^[;#]'){
            $name=$Matches[1].Trim();$key=Get-IniCanonicalKey ($section+'/'+$name)
            if($wanted.ContainsKey($key)){$line=$name+' = '+$wanted[$key];$found[$key]=$true}
        }
        $result.Add($line)
    }
    foreach($key in @($wanted.Keys | Sort-Object)){
        if($found.ContainsKey($key)){continue}
        $parts=$key.Split('/',2);$insert=-1
        for($i=0;$i -lt $result.Count;$i++){
            if($result[$i].Trim() -eq '['+$parts[0]+']'){
                $insert=$i+1
                while($insert -lt $result.Count -and $result[$insert].Trim() -notmatch '^\['){$insert++}
                break
            }
        }
        if($insert -lt 0){$result.Add('');$result.Add('['+$parts[0]+']');$insert=$result.Count}
        $result.Insert($insert,$parts[1]+' = '+$wanted[$key])
    }
    return ,$result.ToArray()
}
