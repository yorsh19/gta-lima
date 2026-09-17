$ErrorActionPreference = "Stop"

$ScriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Path

$GeneratorRoot = Split-Path -Parent $ScriptRoot

$GeoRoot = Join-Path $GeneratorRoot "geo"

$GenerateScript = Join-Path $GeoRoot "generate_city.py"

Write-Host ""
Write-Host "============================================"
Write-Host " KALEX URBAN - GENERATE REAL CITY"
Write-Host "============================================"
Write-Host ""

if (!(Test-Path $GenerateScript)) {
    Write-Host "[ERROR] generate_city.py not found"
    exit 1
}

$Python = Get-Command python -ErrorAction SilentlyContinue

if (!$Python) {

    Write-Host "[ERROR] Python not found in PATH"

    exit 1
}

Write-Host "Python:"
Write-Host $Python.Source

Write-Host ""
Write-Host "Generator:"
Write-Host $GenerateScript
Write-Host ""

& python $GenerateScript

if ($LASTEXITCODE -ne 0) {

    Write-Host ""
    Write-Host "============================================"
    Write-Host " GENERATION FAILED"
    Write-Host "============================================"

    exit $LASTEXITCODE
}

Write-Host ""
Write-Host "============================================"
Write-Host " CITY GENERATED SUCCESSFULLY"
Write-Host "============================================"