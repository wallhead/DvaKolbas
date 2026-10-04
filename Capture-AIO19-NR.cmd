@echo off
powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -File "%~dp0research\nr\tone-contract\runtime-observer\Capture-AioRequest.ps1"
set "captureExit=%errorlevel%"
pause
exit /b %captureExit%
