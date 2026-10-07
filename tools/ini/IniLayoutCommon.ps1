# Package tooling consumes the same internal key mapping as the C++ settings writer.
function Get-IniLayoutAliases {
    $header=Join-Path $PSScriptRoot '../../src/IniLayout.h'
    $table=[regex]::Matches([IO.File]::ReadAllText($header),'Key\{"([^"]+)", "([^"]+)", "([^"]+)", "([^"]+)"\}')
    if(-not $table.Count){throw 'INI key mapping is missing'}
    foreach($row in $table){
        [pscustomobject]@{Legacy=$row.Groups[1].Value+'/'+$row.Groups[2].Value;Canonical=$row.Groups[3].Value+'/'+$row.Groups[4].Value}
    }
}
function Get-PublicIniSchema {
    if(-not $script:PublicIniSchema){
        $script:PublicIniSchema=[IO.File]::ReadAllText((Join-Path $PSScriptRoot 'schema.json')) | ConvertFrom-Json -AsHashtable
    }
    return $script:PublicIniSchema
}
function ConvertFrom-PublicIniValues([hashtable]$Settings) {
    $schema=Get-PublicIniSchema
    $result=@{};foreach($key in $Settings.Keys){$result[$key]=$Settings[$key]}
    $converted=@{}
    foreach($field in $schema.fields){
        $public=$field.section+'/'+$field.key;$internal=$field.internal_section+'/'+$field.internal_key
        if(-not $Settings.ContainsKey($public) -and $field.inherit){continue}
        $value=if($Settings.ContainsKey($public)){[string]$Settings[$public]}else{[string]$field.default}
        $number=0.0
        $numeric=[double]::TryParse($value,[Globalization.NumberStyles]::Float,[Globalization.CultureInfo]::InvariantCulture,[ref]$number) -and [double]::IsFinite($number)
        if(($field.type -eq 'Number' -and -not $numeric) -or
           ($field.type -eq 'Bool' -and $value -cne 'true' -and $value -cne 'false')){
            throw "Invalid [$($field.section)] $($field.key): $value"
        }
        if($field.codec -eq 'MipLodBias'){
            if($value -cne 'Auto' -and -not $numeric){throw 'Invalid [Upscaling] MipLodBias'}
            $converted[$internal]=if($value -ceq 'Auto'){'true'}else{'false'}
        }elseif($field.codec -eq 'Hotkey'){
            $hotkey=@{End=35;Insert=45;Home=36;PageUp=33;PageDown=34;Delete=46;Tab=9}
            if($hotkey.ContainsKey($value)){$value=[string]$hotkey[$value]}
            elseif($value -match '^F([1-9]|1[0-2])$'){$value=[string](111+[int]$Matches[1])}
            elseif($value -match '^0[xX]([0-9a-fA-F]+)$'){$value=[string][Convert]::ToInt32($Matches[1],16)}
            elseif($value -notmatch '^\d+$'){throw "Invalid [$($field.section)] $($field.key)"}
            if([int]$value -lt 1 -or [int]$value -gt 255){throw 'Invalid [Hotkeys] ToggleOverlay'}
            $converted[$internal]=$value
        }elseif($field.values.Count){
            if(-not $field.values.Contains($value)){throw "Invalid [$($field.section)] $($field.key): $value"}
            $converted[$internal]=[string]$field.values[$value]
        }else{$converted[$internal]=$value}
    }
    foreach($field in $schema.fields){$result.Remove($field.internal_section+'/'+$field.internal_key)}
    foreach($key in $converted.Keys){$result[$key]=$converted[$key]}
    $native=if($Settings.ContainsKey('DLSS/Quality')){$Settings['DLSS/Quality'] -ceq 'Native'}else{$true}
    $result['Settings/DLSSNativeScale']=[string]$native.ToString().ToLowerInvariant()
    if($result['Settings/UpscaleType'] -eq '0' -and $native){$result['Settings/UpscaleType']='3'}
    $result['Settings/MipLodBias']=if($result['Settings/UseOptimalMipLodBias'] -eq 'true'){'0.0'}else{$Settings['Upscaling/MipLodBias']}
    $result['Settings/Sharpening']=if([double]::Parse($result['Settings/Sharpness'],[Globalization.CultureInfo]::InvariantCulture) -gt 0){'true'}else{'false'}
    $result['FrameGeneration/Backend']=if($result['Settings/UpscaleType'] -eq '4'){
        if($result['Experimental/FsrOrdinaryPresenter'] -eq 'true'){'0'}else{'2'}
    }else{'1'}
    if($result['FrameGeneration/Backend'] -eq '0' -and $result['FrameGeneration/Enabled'] -eq 'true'){
        throw '[Upscaling Advanced] FsrOrdinaryPresenter requires [FrameGeneration] Enabled=false'
    }
    return $result
}
function ConvertTo-PublicIniValues([hashtable]$Settings) {
    $schema=Get-PublicIniSchema
    $result=@{};foreach($key in $Settings.Keys){$result[$key]=$Settings[$key]}
    $mode=[string]$Settings['Settings/UpscaleType']
    $native=$mode -eq '3' -or ($mode -eq '4' -and $Settings['Settings/DLSSNativeScale'] -eq 'true')
    $converted=@{}
    foreach($field in $schema.fields){
        $internal=$field.internal_section+'/'+$field.internal_key;$public=$field.section+'/'+$field.key
        if(-not $Settings.ContainsKey($internal)){continue}
        $value=[string]$Settings[$internal]
        if($public -eq 'Upscaling/Upscaler'){$value=if($mode -eq '4'){'FSR'}else{'DLSS'}}
        elseif($public -eq 'DLSS/Quality' -and $native){$value='Native'}
        elseif($public -eq 'DLSS/Sharpness' -and $Settings['Settings/Sharpening'] -eq 'false'){$value='0.0'}
        elseif($field.codec -eq 'MipLodBias'){$value=if($value -eq 'true'){'Auto'}else{[string]$Settings['Settings/MipLodBias']}}
        elseif($field.codec -eq 'Hotkey'){
            $code=if($value -match '^0x') {[Convert]::ToInt32($value.Substring(2),16)}else{[int]$value}
            $value=if($code -eq 35){'End'}elseif($code -eq 45){'Insert'}elseif($code -ge 112 -and $code -le 123){'F'+[string]($code-111)}else{'0x'+$code.ToString('X')}
        }elseif($public -eq 'Upscaling Advanced/FsrOrdinaryPresenter'){$value=if($mode -eq '4' -and $Settings['FrameGeneration/Backend'] -eq '0'){'true'}else{'false'}}
        elseif($field.values.Count){
            $found=$false
            foreach($name in $field.values.Keys){
                if($public -eq 'DLSS/Quality' -and $name -eq 'Native'){continue}
                if([string]$field.values[$name] -ceq $value){$value=$name;$found=$true;break}
            }
            if(-not $found){throw "Cannot encode $internal=$value"}
        }
        $converted[$public]=$value
    }
    foreach($field in $schema.fields){$result.Remove($field.internal_section+'/'+$field.internal_key)}
    foreach($alias in Get-IniLayoutAliases){$result.Remove($alias.Legacy)}
    foreach($key in @('Settings/DLSSNativeScale','Settings/Sharpening','Settings/MipLodBias','Settings/ConfigVersion','/ConfigVersion','FrameGeneration/Backend','SourceDLSSG/NRStableColors','NeuralRendering/StableColors')){$result.Remove($key)}
    foreach($key in $converted.Keys){$result[$key]=$converted[$key]}
    return $result
}
function ConvertTo-IniReadView([hashtable]$Settings) {
    if($Settings.ContainsKey('Upscaling/Upscaler')){$Settings=ConvertFrom-PublicIniValues $Settings}
    foreach($alias in Get-IniLayoutAliases){
        $Settings.Remove($alias.Legacy)
        if($Settings.ContainsKey($alias.Canonical)){$Settings[$alias.Legacy]=$Settings[$alias.Canonical]}
    }
    return $Settings
}
function Get-IniCanonicalKey([string]$Key) {
    foreach($alias in Get-IniLayoutAliases){if($alias.Legacy -eq $Key){return $alias.Canonical}}
    return $Key
}
function Format-PublicIni([hashtable]$Values,[string[]]$SourceLines) {
    $schema=Get-PublicIniSchema
    $comments=@{};$section='';$pending=[Collections.Generic.List[string]]::new()
    foreach($line in $SourceLines){
        if($line -match '^\s*\[([^\]]+)\]\s*$'){$section=$Matches[1];$pending.Clear()}
        elseif($line.TrimStart() -match '^[;#]'){$pending.Add($line)}
        elseif($line -match '^\s*([^=]+)=(.*)$'){$comments[$section+'/'+$Matches[1].Trim()]=[string[]]$pending.ToArray();$pending.Clear()}
    }
    $lines=[Collections.Generic.List[string]]::new()
    $lines.Add('; RaZkolbaS settings. Current named layout only.')
    $lines.Add('; [restart] settings require Save and a Skyrim restart. Advanced options are below ordinary controls.')
    foreach($key in @($Values.Keys | Where-Object {$_.StartsWith('/')} | Sort-Object)){
        if($comments.ContainsKey($key)){$lines.AddRange([string[]]$comments[$key])}
        $lines.Add($key.Substring(1)+' = '+[string]$Values[$key])
    }
    $sections=[Collections.Generic.List[string]]::new();foreach($name in $schema.sections){$sections.Add($name)}
    foreach($key in @($Values.Keys | Sort-Object)){$name=$key.Split('/',2)[0];if($name -and -not $sections.Contains($name)){$sections.Add($name)}}
    $known=@{};foreach($field in $schema.fields){$known[$field.section+'/'+$field.key]=$field}
    foreach($section in $sections){
        $keys=@($Values.Keys | Where-Object {$_.Split('/',2)[0] -eq $section})
        if(-not $keys.Count){continue}
        if($section -eq 'Upscaling Advanced'){$lines.Add('');$lines.Add('; ---- Advanced settings ----')}
        if($section -eq 'Menu'){$lines.Add('');$lines.Add('; ---- Program-owned state ----')}
        $lines.Add('');$lines.Add('['+$section+']')
        $ordered=@($schema.fields | Where-Object {$_.section -eq $section} | ForEach-Object {$_.section+'/'+$_.key})
        $ordered+=@($keys | Where-Object {-not $known.ContainsKey($_)} | Sort-Object)
        foreach($key in $ordered){
            if(-not $Values.ContainsKey($key)){continue}
            if($known.ContainsKey($key)){
                $field=$known[$key];$note=if($field.restart){'[restart] '}else{''}
                $choices=if($field.codec -eq 'MipLodBias'){'Auto | numeric bias'}
                    elseif($field.codec -eq 'Hotkey'){'End | Insert | Home | PageUp | PageDown | Delete | Tab | F1-F12 | hex/decimal virtual-key code'}
                    elseif($field.choices){@($field.choices) -join ' | '}
                    elseif($field.type -eq 'Bool'){'true | false'}
                    else{@($field.values.Keys) -join ' | '}
                $guidance='; '+$note+$field.comment
                $lines.Add($guidance)
                if($choices){$lines.Add('; Values: '+$choices+'.')}
                if($field.range){$lines.Add('; Range: '+$field.range+'.')}
            }elseif($comments.ContainsKey($key)){$lines.AddRange([string[]]$comments[$key])}
            $lines.Add($key.Split('/',2)[1]+' = '+[string]$Values[$key])
        }
    }
    return ,$lines.ToArray()
}
function Set-PackageIniValues([string[]]$Lines,[hashtable]$Values) {
    $settings=@{};$section=''
    foreach($line in $Lines){
        if($line.Trim() -match '^\[([^\]]+)\]$'){$section=$Matches[1]}
        elseif($line -match '^\s*([^=]+)=(.*)$' -and $line.TrimStart() -notmatch '^[;#]'){$settings[$section+'/'+$Matches[1].Trim()]=$Matches[2].Trim()}
    }
    $settings=ConvertTo-IniReadView $settings
    foreach($key in $Values.Keys){
        $canonical=Get-IniCanonicalKey $key
        $field=@((Get-PublicIniSchema).fields | Where-Object {$_.section+'/'+$_.key -eq $canonical}) | Select-Object -First 1
        $value=[string]$Values[$key]
        if($field){
            if($field.values.Contains($value)){$value=[string]$field.values[$value]}
            $canonical=$field.internal_section+'/'+$field.internal_key
        }
        $settings[$canonical]=$value
        if($key -eq 'DLSS/Quality'){
            $native=[string]($Values[$key] -ceq 'Native').ToString().ToLowerInvariant()
            $settings['Settings/DLSSNativeScale']=$native
            if($settings['Settings/UpscaleType'] -in @('0','3')){
                $settings['Settings/UpscaleType']=if($native -eq 'true'){'3'}else{'0'}
            }
        }elseif($key -eq 'Upscaling/Upscaler' -and $value -eq '0' -and $settings['Settings/DLSSNativeScale'] -eq 'true'){
            $settings['Settings/UpscaleType']='3'
        }
    }
    if(-not $settings.ContainsKey('Experimental/FsrOrdinaryPresenter')){$settings['Experimental/FsrOrdinaryPresenter']='false'}
    $public=ConvertTo-PublicIniValues $settings
    return ,(Format-PublicIni $public $Lines)
}
