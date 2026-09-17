import json
import urllib.parse
import urllib.request

from config import (
    CENTER_LAT,
    CENTER_LON,
    RADIUS_METERS,
    OVERPASS_URL,
    OSM_RAW_FILE,
    validate_config,
    create_directories
)


def build_query():

    return f"""
[out:json][timeout:120];

(
    way["highway"](around:{RADIUS_METERS},{CENTER_LAT},{CENTER_LON});

    way["building"](around:{RADIUS_METERS},{CENTER_LAT},{CENTER_LON});
    relation["building"](around:{RADIUS_METERS},{CENTER_LAT},{CENTER_LON});

    way["leisure"="park"](around:{RADIUS_METERS},{CENTER_LAT},{CENTER_LON});
    relation["leisure"="park"](around:{RADIUS_METERS},{CENTER_LAT},{CENTER_LON});

    way["leisure"="playground"](around:{RADIUS_METERS},{CENTER_LAT},{CENTER_LON});

    way["landuse"="grass"](around:{RADIUS_METERS},{CENTER_LAT},{CENTER_LON});
    way["landuse"="recreation_ground"](around:{RADIUS_METERS},{CENTER_LAT},{CENTER_LON});

    node["natural"="tree"](around:{RADIUS_METERS},{CENTER_LAT},{CENTER_LON});

    node["amenity"](around:{RADIUS_METERS},{CENTER_LAT},{CENTER_LON});
);

out tags geom;
"""


def download():

    validate_config()
    create_directories()

    query = build_query()

    print("=" * 70)
    print("KALEX URBAN - OSM DOWNLOADER")
    print("=" * 70)

    print(f"Center : {CENTER_LAT}, {CENTER_LON}")
    print(f"Radius : {RADIUS_METERS} m")
    print()
    print("Downloading OpenStreetMap...")

    data = urllib.parse.urlencode({
        "data": query
    }).encode("utf-8")

    request = urllib.request.Request(
        OVERPASS_URL,
        data=data,
        headers={
            "User-Agent": "KalexUrban-MVP/1.0"
        }
    )

    with urllib.request.urlopen(request, timeout=180) as response:
        raw = response.read()

    parsed = json.loads(raw)

    with open(OSM_RAW_FILE, "w", encoding="utf-8") as file:
        json.dump(
            parsed,
            file,
            ensure_ascii=False,
            indent=2
        )

    print()
    print(f"[OK] {len(parsed.get('elements', []))} elementos")
    print(f"[SAVED] {OSM_RAW_FILE}")


if __name__ == "__main__":
    download()