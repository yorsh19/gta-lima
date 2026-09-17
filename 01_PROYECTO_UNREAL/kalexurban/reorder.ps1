$Module = ".\Source\kalexurban"

Write-Host "Reorganizando modulo KalexUrban..." -ForegroundColor Cyan

# ============================================================
# CREAR ESTRUCTURA PUBLIC / PRIVATE
# ============================================================

$Directories = @(
    "$Module\Public",
    "$Module\Public\Data",
    "$Module\Public\Procedural",

    "$Module\Private",
    "$Module\Private\Data",
    "$Module\Private\Procedural"
)

foreach ($Dir in $Directories) {
    New-Item -ItemType Directory -Path $Dir -Force | Out-Null
}

# ============================================================
# HEADERS -> PUBLIC
# ============================================================

$PublicFiles = @{
    "$Module\kalexurban.h"                       = "$Module\Public\kalexurban.h"

    "$Module\Data\LimaTypes.h"                   = "$Module\Public\Data\LimaTypes.h"
    "$Module\Data\LimaDistrictStyle.h"           = "$Module\Public\Data\LimaDistrictStyle.h"

    "$Module\Procedural\LimaRoadSpline.h"        = "$Module\Public\Procedural\LimaRoadSpline.h"
    "$Module\Procedural\LimaBuildingGenerator.h" = "$Module\Public\Procedural\LimaBuildingGenerator.h"
    "$Module\Procedural\LimaUtilityPole.h"       = "$Module\Public\Procedural\LimaUtilityPole.h"
    "$Module\Procedural\LimaCable.h"             = "$Module\Public\Procedural\LimaCable.h"
    "$Module\Procedural\LimaDistrictGenerator.h" = "$Module\Public\Procedural\LimaDistrictGenerator.h"
}

foreach ($Source in $PublicFiles.Keys) {

    $Destination = $PublicFiles[$Source]

    if (Test-Path $Source) {
        Move-Item $Source $Destination -Force
        Write-Host "[PUBLIC]  $Destination" -ForegroundColor Green
    }
}

# ============================================================
# CPP -> PRIVATE
# ============================================================

$PrivateFiles = @{
    "$Module\kalexurban.cpp"                       = "$Module\Private\kalexurban.cpp"

    "$Module\Data\LimaDistrictStyle.cpp"           = "$Module\Private\Data\LimaDistrictStyle.cpp"

    "$Module\Procedural\LimaRoadSpline.cpp"        = "$Module\Private\Procedural\LimaRoadSpline.cpp"
    "$Module\Procedural\LimaBuildingGenerator.cpp" = "$Module\Private\Procedural\LimaBuildingGenerator.cpp"
    "$Module\Procedural\LimaUtilityPole.cpp"       = "$Module\Private\Procedural\LimaUtilityPole.cpp"
    "$Module\Procedural\LimaCable.cpp"             = "$Module\Private\Procedural\LimaCable.cpp"
    "$Module\Procedural\LimaDistrictGenerator.cpp" = "$Module\Private\Procedural\LimaDistrictGenerator.cpp"
}

foreach ($Source in $PrivateFiles.Keys) {

    $Destination = $PrivateFiles[$Source]

    if (Test-Path $Source) {
        Move-Item $Source $Destination -Force
        Write-Host "[PRIVATE] $Destination" -ForegroundColor Yellow
    }
}

# ============================================================
# ELIMINAR DIRECTORIOS ANTIGUOS SI ESTAN VACIOS
# ============================================================

if (Test-Path "$Module\Data") {
    Remove-Item "$Module\Data" -Recurse -Force
}

if (Test-Path "$Module\Procedural") {
    Remove-Item "$Module\Procedural" -Recurse -Force
}

Write-Host ""
Write-Host "==========================================" -ForegroundColor Cyan
Write-Host " ESTRUCTURA C++ REORGANIZADA" -ForegroundColor Green
Write-Host "==========================================" -ForegroundColor Cyan