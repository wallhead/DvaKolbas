$ErrorActionPreference='Stop'
if(Get-Process SkyrimSE,skse64_loader -ErrorAction SilentlyContinue){throw 'Skyrim must be closed for the isolated GPU probe'}
$probe=[IO.Path]::GetFullPath('out/research/fsr4-audit/nvidia-int8-probe')
$records=@()
foreach($pass in @(
 @{name='int8-qualified';policy='MachineLearning';frames=1000;recreate=8;sharp=$false},
 @{name='int8-sharpness';policy='MachineLearning';frames=128;recreate=4;sharp=$true})) {
 $output=Join-Path $probe ('results/'+$pass.name+'.json')
 $arguments=@('--provider',$pass.policy,'--frames',$pass.frames,'--recreate',$pass.recreate,
   '--runtime',('"'+(Join-Path $probe 'runtime')+'"'),'--output',('"'+$output+'"'),'--debug','auto')
 if($pass.sharp){$arguments+=@('--sharpness-check','true')}
 $child=Start-Process -FilePath (Join-Path $probe 'build/Release/NvidiaInt8GpuSmoke.exe') -ArgumentList $arguments -WindowStyle Hidden -PassThru `
  -RedirectStandardOutput (Join-Path $probe ('results/'+$pass.name+'.stdout.log')) `
  -RedirectStandardError (Join-Path $probe ('results/'+$pass.name+'.stderr.log'))
 [void]$child.Handle
 if(-not $child.WaitForExit(120000)){$child.Kill();$child.WaitForExit();throw 'INT8 GPU probe exceeded 120 seconds'}
 $child.Refresh()
 Write-Output "$($pass.name): exit=$($child.ExitCode)"
 Get-Content -LiteralPath (Join-Path $probe ('results/'+$pass.name+'.stdout.log')) -Tail 8
 Get-Content -LiteralPath (Join-Path $probe ('results/'+$pass.name+'.stderr.log')) -Tail 8
 $records+=@{name=$pass.name;exitCode=$child.ExitCode;receiptPath=$output}
}
$records|ConvertTo-Json|Set-Content -LiteralPath (Join-Path $probe 'results/run-summary.json')
if(@($records|Where-Object {$null -eq $_.exitCode -or $_.exitCode -ne 0}).Count){exit 1}
