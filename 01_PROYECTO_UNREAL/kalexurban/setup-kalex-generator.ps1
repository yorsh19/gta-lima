$Root = $PSScriptRoot

Write-Host "============================================"
Write-Host " KALEX URBAN - Procedural Generator Setup"
Write-Host "============================================"

$Directories = @(

    # C++
    "Source",
    "Source\kalexurban",
    "Source\kalexurban\Data",
    "Source\kalexurban\Procedural",

    # UE Content
    "Content\KalexUrban\Core",

    "Content\KalexUrban\Maps",
    "Content\KalexUrban\Maps\Lima_Demo",
    "Content\KalexUrban\Maps\TestMaps",

    "Content\KalexUrban\Procedural",

    "Content\KalexUrban\Procedural\Blueprints",
    "Content\KalexUrban\Procedural\Blueprints\District",
    "Content\KalexUrban\Procedural\Blueprints\Roads",
    "Content\KalexUrban\Procedural\Blueprints\Buildings",
    "Content\KalexUrban\Procedural\Blueprints\Utilities",

    "Content\KalexUrban\Procedural\PCG",
    "Content\KalexUrban\Procedural\PCG\Roads",
    "Content\KalexUrban\Procedural\PCG\Lots",
    "Content\KalexUrban\Procedural\PCG\Buildings",
    "Content\KalexUrban\Procedural\PCG\Utilities",
    "Content\KalexUrban\Procedural\PCG\Decoration",

    "Content\KalexUrban\Procedural\Data",
    "Content\KalexUrban\Procedural\Data\Districts",
    "Content\KalexUrban\Procedural\Data\Buildings",

    "Content\KalexUrban\Environment",

    "Content\KalexUrban\Environment\Buildings",
    "Content\KalexUrban\Environment\Buildings\Modular",
    "Content\KalexUrban\Environment\Buildings\Hero",

    "Content\KalexUrban\Environment\Roads",

    "Content\KalexUrban\Environment\Infrastructure",
    "Content\KalexUrban\Environment\Infrastructure\Poles",
    "Content\KalexUrban\Environment\Infrastructure\Cables",
    "Content\KalexUrban\Environment\Infrastructure\Lights",

    "Content\KalexUrban\Environment\Props",
    "Content\KalexUrban\Environment\Vegetation",
    "Content\KalexUrban\Environment\Landscape",

    "Content\KalexUrban\Materials",
    "Content\KalexUrban\Materials\Master",
    "Content\KalexUrban\Materials\Instances",
    "Content\KalexUrban\Materials\Functions",
    "Content\KalexUrban\Materials\Textures"
)

foreach ($Directory in $Directories) {

    $FullPath = Join-Path $Root $Directory

    if (!(Test-Path $FullPath)) {
        New-Item -ItemType Directory -Path $FullPath -Force | Out-Null
        Write-Host "[CREATED] $Directory"
    }
    else {
        Write-Host "[EXISTS]  $Directory"
    }
}

$Files = @(

    "Source\kalexurban.Target.cs",
    "Source\kalexurbanEditor.Target.cs",

    "Source\kalexurban\kalexurban.Build.cs",
    "Source\kalexurban\kalexurban.h",
    "Source\kalexurban\kalexurban.cpp",

    "Source\kalexurban\Data\LimaTypes.h",
    "Source\kalexurban\Data\LimaDistrictStyle.h",
    "Source\kalexurban\Data\LimaDistrictStyle.cpp",

    "Source\kalexurban\Procedural\LimaRoadSpline.h",
    "Source\kalexurban\Procedural\LimaRoadSpline.cpp",

    "Source\kalexurban\Procedural\LimaBuildingGenerator.h",
    "Source\kalexurban\Procedural\LimaBuildingGenerator.cpp",

    "Source\kalexurban\Procedural\LimaUtilityPole.h",
    "Source\kalexurban\Procedural\LimaUtilityPole.cpp",

    "Source\kalexurban\Procedural\LimaCable.h",
    "Source\kalexurban\Procedural\LimaCable.cpp",

    "Source\kalexurban\Procedural\LimaDistrictGenerator.h",
    "Source\kalexurban\Procedural\LimaDistrictGenerator.cpp"
)

foreach ($File in $Files) {

    $FullPath = Join-Path $Root $File

    if (!(Test-Path $FullPath)) {
        New-Item -ItemType File -Path $FullPath -Force | Out-Null
        Write-Host "[CREATED] $File"
    }
    else {
        Write-Host "[EXISTS]  $File"
    }
}

Write-Host ""
Write-Host "KALEX Urban structure created."