from pathlib import Path


SCRIPT_DIR = Path(__file__).resolve().parent

GENERATOR_DIR = SCRIPT_DIR.parent

ASSETS_ROOT = GENERATOR_DIR.parent


OUTPUT_3D = ASSETS_ROOT / "3D"


BUILDINGS = OUTPUT_3D / "edificios"

BUILDING_MODULAR = BUILDINGS / "modular"
BUILDING_DOORS = BUILDINGS / "puertas"
BUILDING_WINDOWS = BUILDINGS / "ventanas"
BUILDING_ROOFS = BUILDINGS / "techos"
BUILDING_PROPS = BUILDINGS / "props"


INFRASTRUCTURE = OUTPUT_3D / "infraestructura"

ROADS = INFRASTRUCTURE / "pistas"
SIDEWALKS = INFRASTRUCTURE / "veredas"
POLES = INFRASTRUCTURE / "postes"
CABLES = INFRASTRUCTURE / "cables"
STREETLIGHTS = INFRASTRUCTURE / "alumbrado"


PROPS = OUTPUT_3D / "props"

CONSTRUCTION_PROPS = PROPS / "construccion"
STREET_PROPS = PROPS / "calle"
COMMERCIAL_PROPS = PROPS / "comercio"


ALL_OUTPUT_DIRS = [
    BUILDING_MODULAR,
    BUILDING_DOORS,
    BUILDING_WINDOWS,
    BUILDING_ROOFS,
    BUILDING_PROPS,

    ROADS,
    SIDEWALKS,
    POLES,
    CABLES,
    STREETLIGHTS,

    CONSTRUCTION_PROPS,
    STREET_PROPS,
    COMMERCIAL_PROPS
]


def ensure_output_directories():

    for directory in ALL_OUTPUT_DIRS:
        directory.mkdir(
            parents=True,
            exist_ok=True
        )