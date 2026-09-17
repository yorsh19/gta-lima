from common.mesh_utils import (
    create_box,
    join_objects
)

from common.export_utils import export_fbx

from config import BUILDING_WINDOWS


def generate(materials):

    parts = []

    width = 1.40
    height = 1.20

    # marco
    parts.append(
        create_box(
            "Top",
            (width, 0.08, 0.06),
            (0, 0, height / 2),
            materials["metal"]
        )
    )

    parts.append(
        create_box(
            "Bottom",
            (width, 0.08, 0.06),
            (0, 0, -height / 2),
            materials["metal"]
        )
    )

    parts.append(
        create_box(
            "Left",
            (0.06, 0.08, height),
            (-width / 2, 0, 0),
            materials["metal"]
        )
    )

    parts.append(
        create_box(
            "Right",
            (0.06, 0.08, height),
            (width / 2, 0, 0),
            materials["metal"]
        )
    )

    # rejas verticales
    for x in [
        -0.50,
        -0.25,
        0,
        0.25,
        0.50
    ]:

        parts.append(
            create_box(
                "Grille",
                (
                    0.025,
                    0.10,
                    height
                ),
                (
                    x,
                    -0.02,
                    0
                ),
                materials["metal"]
            )
        )

    # rejas horizontales
    for z in [
        -0.30,
        0,
        0.30
    ]:

        parts.append(
            create_box(
                "Grille",
                (
                    width,
                    0.10,
                    0.025
                ),
                (
                    0,
                    -0.02,
                    z
                ),
                materials["metal"]
            )
        )

    result = join_objects(
        parts,
        "SM_SJL_Window_Grille_01"
    )

    export_fbx(
        result,
        BUILDING_WINDOWS
    )

    result.hide_set(True)