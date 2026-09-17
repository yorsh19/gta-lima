import sys
import traceback
from pathlib import Path


BLENDER_ROOT = Path(__file__).resolve().parent

if str(BLENDER_ROOT) not in sys.path:
    sys.path.insert(0, str(BLENDER_ROOT))


from config import ensure_output_directories

from common.mesh_utils import clear_scene
from common.material_utils import create_default_materials

import buildings.walls as walls
import buildings.doors as doors
import buildings.windows as windows
import buildings.roofs as roofs
import buildings.building_props as building_props

import roads.roads as road_meshes
import roads.sidewalks as sidewalks
import roads.curbs as curbs

import infrastructure.poles as poles
import infrastructure.cables as cables
import infrastructure.streetlights as streetlights


def execute_generator(name, generator, materials):

    print("")
    print("=" * 60)
    print(f"GENERATING: {name}")
    print("=" * 60)

    clear_scene()

    generator(materials)

    print(f"[OK] {name}")


def main():

    print("")
    print("=" * 60)
    print("KALEX URBAN")
    print("SJL PROCEDURAL ASSET GENERATOR")
    print("=" * 60)

    ensure_output_directories()

    materials = create_default_materials()

    generators = [
        ("BUILDING WALLS", walls.generate),
        ("DOORS", doors.generate),
        ("WINDOWS", windows.generate),
        ("ROOFS", roofs.generate),
        ("BUILDING PROPS", building_props.generate),

        ("ROADS", road_meshes.generate),
        ("SIDEWALKS", sidewalks.generate),
        ("CURBS", curbs.generate),

        ("UTILITY POLES", poles.generate),
        ("CABLES", cables.generate),
        ("STREET LIGHTS", streetlights.generate),
    ]

    generated = 0

    for name, generator in generators:

        execute_generator(
            name,
            generator,
            materials
        )

        generated += 1

    clear_scene()

    print("")
    print("=" * 60)
    print("KALEX URBAN GENERATION COMPLETE")
    print(f"GENERATORS EXECUTED: {generated}")
    print("=" * 60)


if __name__ == "__main__":

    try:
        main()

    except Exception:

        print("")
        print("=" * 60)
        print("KALEX URBAN GENERATION FAILED")
        print("=" * 60)

        traceback.print_exc()

        sys.exit(1)