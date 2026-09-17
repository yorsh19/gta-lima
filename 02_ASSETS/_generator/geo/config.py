from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parents[3]

CITY_NAME = "SJL_CantoGrande_MVP"

CITY_ROOT = PROJECT_ROOT / "03_CITY_DATA" / CITY_NAME
RAW_ROOT = CITY_ROOT / "raw"
OSM_ROOT = RAW_ROOT / "osm"
PROCESSED_ROOT = CITY_ROOT / "processed"

# ============================================================
# CENTRO DEL MVP
# ============================================================
#
# Copia las coordenadas del punto central de tu zona.
#
# Google Maps:
# clic derecho -> "¿Qué hay aquí?"
#
# Ejemplo:
# CENTER_LAT = -12.xxxxx
# CENTER_LON = -77.xxxxx
#
# NO uses estas variables sin colocar tus coordenadas reales.
#

# ============================================================
# CENTRO DEL MVP - C. JORGE SEDANO / CANTO GRANDE
# ============================================================

CENTER_LAT = -11.974524
CENTER_LON = -77.011986

# Radio inicial.
# 500 m = aproximadamente 1 km x 1 km.
RADIUS_METERS = 500

OVERPASS_URL = "https://overpass-api.de/api/interpreter"

OSM_RAW_FILE = OSM_ROOT / "sjl_osm.json"
CITY_OUTPUT_FILE = PROCESSED_ROOT / "city.json"


def validate_config():

    if CENTER_LAT is None or CENTER_LON is None:
        raise RuntimeError(
            "\n"
            "Debes configurar CENTER_LAT y CENTER_LON en:\n"
            f"{Path(__file__).resolve()}\n"
        )


def create_directories():

    OSM_ROOT.mkdir(parents=True, exist_ok=True)
    PROCESSED_ROOT.mkdir(parents=True, exist_ok=True)