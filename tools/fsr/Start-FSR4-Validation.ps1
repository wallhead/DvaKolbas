$ErrorActionPreference='Stop'
# Portable standalone kit. It does not edit Skyrim, MO2, INIs or driver settings.
try {
    if(Get-Process SkyrimSE,skse64_loader -ErrorAction SilentlyContinue){throw 'Close Skyrim before running standalone GPU validation.'}
    $manifest=Get-Content -LiteralPath (Join-Path $PSScriptRoot 'kit-manifest.json') -Raw | ConvertFrom-Json
    foreach($file in $manifest.files) {
        if($file.path -notmatch '^(TRPFsrGpuSmoke\.exe|runtime/amd_fidelityfx_(loader|upscaler)_dx12\.dll)$'){throw 'Unexpected validation payload path.'}
        $path=Join-Path $PSScriptRoot $file.path
        if((Get-Item -LiteralPath $path).Length -ne $file.bytes -or
           (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToLowerInvariant() -ne $file.sha256){throw "Validation payload mismatch: $($file.path)"}
    }
    $run=Join-Path $PSScriptRoot ('results/'+[DateTime]::Now.ToString('yyyyMMdd-HHmmss')+'-'+[guid]::NewGuid().ToString('N').Substring(0,8))
    [IO.Directory]::CreateDirectory($run)|Out-Null
    $exe=Join-Path $PSScriptRoot 'TRPFsrGpuSmoke.exe'
    $runtime=Join-Path $PSScriptRoot 'runtime'
    $qualified=$true
    foreach($pass in @(
        @{Name='fsr3';Policy='Analytical';Frames=128;Recreate=4;Sharpness=$false},
        @{Name='auto';Policy='Compatible';Frames=128;Recreate=4;Sharpness=$false},
        @{Name='fsr4';Policy='MachineLearning';Frames=1000;Recreate=8;Sharpness=$false},
        @{Name='fsr4-sharpness';Policy='MachineLearning';Frames=128;Recreate=4;Sharpness=$true})) {
        Write-Host "Running $($pass.Name)..."
        $output=Join-Path $run ($pass.Name+'.json')
        $arguments=@('--provider',$pass.Policy,'--frames',$pass.Frames,'--recreate',$pass.Recreate,
            '--runtime',('"'+$runtime+'"'),'--output',('"'+$output+'"'),'--debug','auto')
        if($pass.Sharpness){$arguments+=@('--sharpness-check','true')}
        $process=Start-Process -FilePath $exe -ArgumentList $arguments -WindowStyle Hidden -PassThru `
            -RedirectStandardOutput (Join-Path $run ($pass.Name+'.stdout.log')) `
            -RedirectStandardError (Join-Path $run ($pass.Name+'.stderr.log'))
        # Windows PowerShell 5 must retain the native process handle before a
        # short-lived child exits, otherwise ExitCode can be null after waiting.
        [void]$process.Handle
        if(-not $process.WaitForExit(120000)) {
            $process.Kill();$process.WaitForExit();throw "GPU check timed out: $($pass.Name). Send the results folder."
        }
        $process.Refresh()
        if($process.ExitCode -eq 77){$qualified=$false;Write-Host 'NOT QUALIFIED: required runtime/adapter support unavailable.';continue}
        if($null -eq $process.ExitCode -or $process.ExitCode -ne 0){throw "GPU check failed: $($pass.Name), exit=$($process.ExitCode). Send the results folder."}
        $result=Get-Content -LiteralPath $output -Raw | ConvertFrom-Json
        if($result.result -ne 'PASS'){throw "Missing successful receipt: $($pass.Name)"}
        if($pass.Policy -eq 'MachineLearning' -and $result.providerName -notmatch '^4\.'){throw 'Explicit FSR4 test used another provider.'}
        Write-Host "PASS: actual provider $($result.providerName), $($result.frames) frames."
    }
    Write-Host "Results: $run"
    if(-not $qualified){Write-Host 'NOT QUALIFIED: FSR4 hardware execution remains unverified.';exit 77}
    Write-Host 'PASS: standalone FSR4 checks. Skyrim and FSR4 + FG gameplay remain untested.'
    exit 0
} catch {Write-Host $_.Exception.Message;exit 1}
