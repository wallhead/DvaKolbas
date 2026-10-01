$ErrorActionPreference = 'Stop'
$workspace = Split-Path $PSScriptRoot -Parent
$log = Join-Path $workspace 'out\validation\graphics-tools-install-with-update.json'
$result = [ordered]@{ succeeded=$false; stage='administrator check'; originalUpdateStartMode=$null; originalUpdateState=$null; restoredUpdateSetting=$false }
$updateChanged = $false
try {
    $principal = [Security.Principal.WindowsPrincipal]::new([Security.Principal.WindowsIdentity]::GetCurrent())
    if (-not $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) { throw 'Installer must run as administrator.' }
    $result.stage = 'check Graphics Tools'
    $capability = Get-WindowsCapability -Online -Name 'Tools.Graphics.DirectX~~~~0.0.1.0'
    $result.before = [string]$capability.State
    if ($capability.State -ne 'Installed') {
        $service = Get-CimInstance Win32_Service -Filter "Name='wuauserv'"
        $result.originalUpdateStartMode = $service.StartMode
        $result.originalUpdateState = $service.State
        $result.stage = 'start Windows Update temporarily'
        if ($service.StartMode -eq 'Disabled') {
            Set-Service -Name wuauserv -StartupType Manual
            $updateChanged = $true
        }
        if ($service.State -ne 'Running') {
            Start-Service -Name wuauserv
            $updateChanged = $true
        }
        $result.stage = 'install Graphics Tools'
        $installation = Add-WindowsCapability -Online -Name $capability.Name
        $result.restartNeeded = [bool]$installation.RestartNeeded
    }
    $result.stage = 'verify installation'
    $after = Get-WindowsCapability -Online -Name 'Tools.Graphics.DirectX~~~~0.0.1.0'
    $result.after = [string]$after.State
    $result.succeeded = ($after.State -eq 'Installed')
} catch {
    $result.error = $_.Exception.Message
} finally {
    if ($updateChanged) {
        try {
            if ($result.originalUpdateState -ne 'Running') { Stop-Service -Name wuauserv }
            $originalStartType = switch ($result.originalUpdateStartMode) { 'Disabled' { 'Disabled' }; 'Auto' { 'Automatic' }; default { 'Manual' } }
            Set-Service -Name wuauserv -StartupType $originalStartType
            $result.restoredUpdateSetting = $true
        } catch { $result.restoreError = $_.Exception.Message }
    }
    $result | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath $log -Encoding utf8
}
if (-not $result.succeeded) { exit 1 }
