$AssetsRoot = Resolve-Path "$PSScriptRoot\..\.."

Write-Host ""
Write-Host "========================================"
Write-Host " KALEX URBAN - ASSET GENERATOR"
Write-Host "========================================"
Write-Host ""

$Directories = @(

    "3D\edificios\modular",
    "3D\edificios\puertas",
    "3D\edificios\ventanas",
    "3D\edificios\techos",
    "3D\edificios\props",

    "3D\infraestructura\pistas",
    "3D\infraestructura\veredas",
    "3D\infraestructura\postes",
    "3D\infraestructura\cables",
    "3D\infraestructura\alumbrado",

    "3D\props\construccion",
    "3D\props\calle",
    "3D\props\comercio",

    "_generator\blender",
    "_generator\blender\common",
    "_generator\blender\buildings",
    "_generator\blender\roads",
    "_generator\blender\infrastructure",
    "_generator\powershell"
)

foreach ($Directory in $Directories)
{
    $Path = Join-Path $AssetsRoot $Directory

    if (!(Test-Path $Path))
    {
        New-Item `
            -ItemType Directory `
            -Path $Path `
            -Force | Out-Null

        Write-Host "[CREATED] $Directory"
    }
    else
    {
        Write-Host "[EXISTS]  $Directory"
    }
}

Write-Host ""
Write-Host "Structure ready."