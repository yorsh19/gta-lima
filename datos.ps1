$root = "C:\Users\yorsh\Documents\kalex\kalex-urban"

$folders = @(
    "$root\02_ASSETS\_generator\geo",
    "$root\03_CITY_DATA\SJL_CantoGrande_MVP\raw\osm",
    "$root\03_CITY_DATA\SJL_CantoGrande_MVP\processed",
    "$root\03_CITY_DATA\SJL_CantoGrande_MVP\config"
)

foreach ($folder in $folders) {
    New-Item -ItemType Directory -Force -Path $folder | Out-Null
}

$files = @(
    "$root\02_ASSETS\_generator\geo\config.py",
    "$root\02_ASSETS\_generator\geo\download_osm.py",
    "$root\02_ASSETS\_generator\geo\coordinate_converter.py",
    "$root\02_ASSETS\_generator\geo\process_osm.py",
    "$root\02_ASSETS\_generator\geo\generate_city.py",
    "$root\02_ASSETS\_generator\powershell\generate_city.ps1"
)

foreach ($file in $files) {
    if (!(Test-Path $file)) {
        New-Item -ItemType File -Path $file | Out-Null
    }
}

Write-Host "KALEX CITY GENERATOR creado."