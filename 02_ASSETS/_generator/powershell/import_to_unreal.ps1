$ErrorActionPreference = "Stop"


$ProjectRoot = Resolve-Path "$PSScriptRoot\..\..\..\01_PROYECTO_UNREAL\kalexurban"

$Project = Join-Path `
    $ProjectRoot `
    "kalexurban.uproject"


$ImportScript = Resolve-Path `
    "$PSScriptRoot\..\unreal\import_generated_assets.py"


$UnrealEditorCmd = `
    "C:\Program Files\Epic Games\UE_5.6\Engine\Binaries\Win64\UnrealEditor-Cmd.exe"


Write-Host ""
Write-Host "========================================"
Write-Host " KALEX URBAN - IMPORT TO UE5"
Write-Host "========================================"
Write-Host ""


if (!(Test-Path $UnrealEditorCmd))
{
    Write-Host "ERROR: UnrealEditor-Cmd.exe not found:"
    Write-Host $UnrealEditorCmd
    exit 1
}


if (!(Test-Path $Project))
{
    Write-Host "ERROR: Unreal project not found:"
    Write-Host $Project
    exit 1
}


Write-Host "Project:"
Write-Host $Project

Write-Host ""

Write-Host "Python:"
Write-Host $ImportScript

Write-Host ""


$SuccessMarker = Join-Path $PSScriptRoot "..\validation\import_success.json"
if (Test-Path -LiteralPath $SuccessMarker) {
    Remove-Item -LiteralPath $SuccessMarker
}

& $UnrealEditorCmd `
    $Project `
    -ExecutePythonScript="$ImportScript" `
    -unattended `
    -nop4 `
    -nosplash


$ExitCode = $LASTEXITCODE


if ($ExitCode -ne 0 -or !(Test-Path -LiteralPath $SuccessMarker))
{
    Write-Host ""
    Write-Host "========================================"
    Write-Host " IMPORT FAILED"
    Write-Host " Unreal exit code: $ExitCode"
    Write-Host "========================================"

    if ($ExitCode -eq 0) { $ExitCode = 1 }
    exit $ExitCode
}


Write-Host ""
Write-Host "========================================"
Write-Host " IMPORT COMPLETED SUCCESSFULLY"
Write-Host "========================================"
