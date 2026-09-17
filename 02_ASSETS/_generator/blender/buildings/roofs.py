from common.mesh_utils import (
    create_box,
    create_cylinder,
    join_objects
)

from common.export_utils import export_fbx
from common.mesh_utils import set_origin_ground_center

from config import BUILDING_ROOFS


def generate(materials):

    roof = create_box(
        "SM_SJL_Roof_Flat_01",
        (
            6.0,
            12.0,
            0.20
        ),
        (
            0,
            0,
            0.10
        ),
        materials["roof"]
    )

    set_origin_ground_center(roof)
    export_fbx(
        roof,
        BUILDING_ROOFS
    )

    roof.hide_set(True)

    # techo inconcluso con fierros
    parts = []

    slab = create_box(
        "Slab",
        (
            6.0,
            12.0,
            0.20
        ),
        (
            0,
            0,
            0.10
        ),
        materials["roof"]
    )

    parts.append(slab)

    positions = [
        (-2.7, -5.5),
        (2.7, -5.5),
        (-2.7, 5.5),
        (2.7, 5.5)
    ]

    for x, y in positions:

        for offset in [
            -0.05,
            0.05
        ]:

            rod = create_cylinder(
                "Rebar",
                0.012,
                1.2,
                (
                    x + offset,
                    y,
                    0.8
                ),
                8,
                materials["metal"]
            )

            parts.append(rod)

    result = join_objects(
        parts,
        "SM_SJL_Roof_Unfinished_01"
    )

    set_origin_ground_center(result)
    export_fbx(
        result,
        BUILDING_ROOFS
    )

    result.hide_set(True)