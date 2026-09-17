$ErrorActionPreference = "Stop"
$Python = (Get-Command python).Source
Write-Host "Installing KALEX Urban geo dependencies with $Python"
& $Python -m pip install -r "$PSScriptRoot\geo\requirements.txt"
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
Write-Host "[OK] Geo dependencies installed"
