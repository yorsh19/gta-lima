$ErrorActionPreference = "Stop"
$ProjectDir = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $ProjectDir
Remove-Item ".\Binaries" -Recurse -Force -ErrorAction SilentlyContinue
Remove-Item ".\Intermediate" -Recurse -Force -ErrorAction SilentlyContinue
& "C:\Program Files\Epic Games\UE_5.6\Engine\Build\BatchFiles\Build.bat" `
    kalexurbanEditor `
    Win64 `
    Development `
    "$ProjectDir\kalexurban.uproject" `
    -WaitMutex
