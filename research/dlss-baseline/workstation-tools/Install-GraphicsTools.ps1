$ErrorActionPreference = 'Stop'
$workspace = Split-Path $PSScriptRoot -Parent
$log = Join-Path $workspace 'out\validation\graphics-tools-install.json'
try {
    $capability = Get-WindowsCapability -Online -Name 'Tools.Graphics.DirectX~~~~0.0.1.0'
    $before = [string]$capability.State
    $installation = if ($capability.State -ne 'Installed') { Add-WindowsCapability -Online -Name $capability.Name } else { $null }
    $after = Get-WindowsCapability -Online -Name 'Tools.Graphics.DirectX~~~~0.0.1.0'
    [ordered]@{ before=$before; after=[string]$after.State; restartNeeded=[bool]$installation.RestartNeeded; succeeded=($after.State -eq 'Installed') } | ConvertTo-Json | Set-Content -LiteralPath $log -Encoding utf8
} catch {
    [ordered]@{ succeeded=$false; error=$_.Exception.Message } | ConvertTo-Json | Set-Content -LiteralPath $log -Encoding utf8
    exit 1
}
