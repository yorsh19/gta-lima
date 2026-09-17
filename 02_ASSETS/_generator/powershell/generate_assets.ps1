$ErrorActionPreference = "Stop"

$GeneratorRoot = Resolve-Path "$PSScriptRoot\.."

$PythonScript = Join-Path `
    $GeneratorRoot `
    "blender\generate_all.py"


Write-Host ""
Write-Host "========================================"
Write-Host " KALEX URBAN - GENERATE SJL ASSETS"
Write-Host "========================================"
Write-Host ""


$PossibleBlenderPaths = @(

    "C:\Program Files\Blender Foundation\Blender 5.2\blender.exe",

    "C:\Program Files\Blender Foundation\Blender 5.1\blender.exe",

    "C:\Program Files\Blender Foundation\Blender 5.0\blender.exe",

    "C:\Program Files\Blender Foundation\Blender 4.5\blender.exe",

    "C:\Program Files\Blender Foundation\Blender 4.4\blender.exe"
)


$Blender = $null


foreach ($Candidate in $PossibleBlenderPaths)
{
    if (Test-Path $Candidate)
    {
        $Blender = $Candidate
        break
    }
}


if (!$Blender)
{
    Write-Host "ERROR: Blender not found."
    exit 1
}


if (!(Test-Path $PythonScript))
{
    Write-Host "ERROR: generate_all.py not found:"
    Write-Host $PythonScript
    exit 1
}


Write-Host "Blender:"
Write-Host $Blender

Write-Host ""

Write-Host "Generator:"
Write-Host $PythonScript

Write-Host ""


& $Blender `
    --background `
    --python-exit-code 1 `
    --python $PythonScript


$BlenderExitCode = $LASTEXITCODE


if ($BlenderExitCode -ne 0)
{
    Write-Host ""
    Write-Host "========================================"
    Write-Host " GENERATION FAILED"
    Write-Host " Blender exit code: $BlenderExitCode"
    Write-Host "========================================"

    exit $BlenderExitCode
}


Write-Host ""
Write-Host "========================================"
Write-Host " ASSETS GENERATED SUCCESSFULLY"
Write-Host "========================================"