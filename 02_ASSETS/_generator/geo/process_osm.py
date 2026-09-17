import json
import math

from config import (
    CENTER_LAT,
    CENTER_LON,
    OSM_RAW_FILE,
    CITY_OUTPUT_FILE,
    validate_config,
    create_directories
)

from coordinate_converter import LocalCoordinateConverter
from urban_geometry import derive_blocks, derive_lots, add_oriented_building_fields


ROAD_WIDTHS = {

    "motorway": 12.0,
    "trunk": 11.0,

    "primary": 10.0,
    "secondary": 9.0,
    "tertiary": 8.0,

    "residential": 6.5,
    "living_street": 5.5,

    "service": 4.5,
    "unclassified": 5.5,

    "track": 4.0,
    "path": 2.0,
    "footway": 2.0
}


DEFAULT_BUILDING_FLOORS = 2


def parse_int(value, default):

    try:
        return int(value)
    except:
        return default


def calculate_center(points):

    if not points:
        return [0.0, 0.0]

    x = sum(p[0] for p in points) / len(points)
    y = sum(p[1] for p in points) / len(points)

    return [x, y]


def process():

    validate_config()
    create_directories()

    print("=" * 70)
    print("KALEX URBAN - PROCESS OSM")
    print("=" * 70)

    with open(OSM_RAW_FILE, "r", encoding="utf-8") as file:
        osm = json.load(file)

    converter = LocalCoordinateConverter(
        CENTER_LAT,
        CENTER_LON
    )

    roads = []
    buildings = []
    parks = []
    trees = []
    amenities = []

    for element in osm.get("elements", []):

        tags = element.get("tags", {})
        geometry = element.get("geometry", [])

        # ====================================================
        # ROADS
        # ====================================================

        if element["type"] == "way" and "highway" in tags:

            highway_type = tags.get(
                "highway",
                "residential"
            )

            points = []

            for node in geometry:

                x, y = converter.to_local(
                    node["lat"],
                    node["lon"]
                )

                points.append([
                    round(x, 3),
                    round(y, 3),
                    0.0
                ])

            if len(points) < 2:
                continue

            lanes = parse_int(
                tags.get("lanes"),
                2
            )

            width = tags.get("width")

            try:
                width = float(width)
            except:
                width = ROAD_WIDTHS.get(
                    highway_type,
                    6.0
                )

            roads.append({

                "id": element["id"],

                "name": tags.get(
                    "name",
                    ""
                ),

                "type": highway_type,

                "surface": tags.get(
                    "surface",
                    "unknown"
                ),

                "lanes": lanes,

                "width": width,

                "oneway":
                    tags.get("oneway") == "yes",

                "points": points
            })

            continue

        # ====================================================
        # BUILDINGS
        # ====================================================

        if element["type"] == "way" and "building" in tags:

            footprint = []

            for node in geometry:

                x, y = converter.to_local(
                    node["lat"],
                    node["lon"]
                )

                footprint.append([
                    round(x, 3),
                    round(y, 3)
                ])

            if len(footprint) < 3:
                continue

            # OSM normalmente cierra el polígono
            if footprint[0] == footprint[-1]:
                footprint.pop()

            floors = parse_int(
                tags.get("building:levels"),
                DEFAULT_BUILDING_FLOORS
            )

            height = tags.get("height")

            try:
                height = float(
                    str(height)
                    .replace("m", "")
                    .strip()
                )
            except:
                height = floors * 3.0

            buildings.append(add_oriented_building_fields({

                "id": element["id"],

                "type": tags.get(
                    "building",
                    "yes"
                ),

                "name": tags.get(
                    "name",
                    ""
                ),

                "floors": floors,

                "height": height,

                "center":
                    calculate_center(footprint),

                "footprint":
                    footprint
            }))

            continue

        # ====================================================
        # PARKS
        # ====================================================

        if (
            element["type"] == "way"
            and (
                tags.get("leisure") in (
                    "park",
                    "playground"
                )
                or tags.get("landuse") in (
                    "grass",
                    "recreation_ground"
                )
            )
        ):

            polygon = []

            for node in geometry:

                x, y = converter.to_local(
                    node["lat"],
                    node["lon"]
                )

                polygon.append([
                    round(x, 3),
                    round(y, 3)
                ])

            if polygon and polygon[0] == polygon[-1]:
                polygon.pop()

            if len(polygon) >= 3:

                parks.append({

                    "id": element["id"],

                    "name":
                        tags.get("name", ""),

                    "type":
                        tags.get(
                            "leisure",
                            tags.get(
                                "landuse",
                                "park"
                            )
                        ),

                    "polygon": polygon
                })

            continue

        # ====================================================
        # TREES
        # ====================================================

        if (
            element["type"] == "node"
            and tags.get("natural") == "tree"
        ):

            lat = element.get("lat")
            lon = element.get("lon")

            if lat is not None and lon is not None:

                x, y = converter.to_local(
                    lat,
                    lon
                )

                trees.append({
                    "id": element["id"],
                    "position": [
                        round(x, 3),
                        round(y, 3),
                        0.0
                    ]
                })

        # ====================================================
        # AMENITIES
        # ====================================================

        if (
            element["type"] == "node"
            and "amenity" in tags
        ):

            lat = element.get("lat")
            lon = element.get("lon")

            if lat is not None and lon is not None:

                x, y = converter.to_local(
                    lat,
                    lon
                )

                amenities.append({

                    "id": element["id"],

                    "type":
                        tags["amenity"],

                    "name":
                        tags.get("name", ""),

                    "position": [
                        round(x, 3),
                        round(y, 3),
                        0.0
                    ]
                })

    # Derive closed urban blocks from the OSM road graph and conservative lots.
    blocks = derive_blocks(roads, parks, clip_radius=RADIUS_METERS + 30.0)
    lots = derive_lots(blocks)

    city = {

        "version": 2,

        "name": "SJL Canto Grande MVP",

        "coordinateSystem":
            "LocalMeters",

        "origin": {
            "latitude": CENTER_LAT,
            "longitude": CENTER_LON
        },

        "roads": roads,
        "blocks": blocks,
        "lots": lots,
        "buildings": buildings,
        "parks": parks,
        "trees": trees,
        "amenities": amenities
    }

    with open(
        CITY_OUTPUT_FILE,
        "w",
        encoding="utf-8"
    ) as file:

        json.dump(
            city,
            file,
            ensure_ascii=False,
            indent=2
        )

    print()
    print("RESULT:")
    print(f"Roads      : {len(roads)}")
    print(f"Blocks     : {len(blocks)}")
    print(f"Lots       : {len(lots)}")
    print(f"Buildings  : {len(buildings)}")
    print(f"Parks      : {len(parks)}")
    print(f"Trees      : {len(trees)}")
    print(f"Amenities  : {len(amenities)}")

    print()
    print(f"[GENERATED] {CITY_OUTPUT_FILE}")


if __name__ == "__main__":
    process()