from common.mesh_utils import (
    create_cylinder,
    create_box,
    join_objects
)

from common.export_utils import export_fbx
from common.mesh_utils import (
    create_cylinder,
    create_box,
    join_objects,
    set_origin_bottom
)

from config import POLES


def generate(materials):

    parts = []

    pole = create_cylinder(
        "Pole",
        radius=0.13,
        depth=8.5,
        location=(
            0,
            0,
            4.25
        ),
        vertices=16,
        material=materials["concrete"]
    )

    parts.append(pole)

    crossbar = create_box(
        "Crossbar",
        (
            1.8,
            0.12,
            0.14
        ),
        (
            0,
            0,
            7.8
        ),
        materials["concrete"]
    )

    parts.append(crossbar)

    for x in [
        -0.70,
        0,
        0.70
    ]:

        insulator = create_cylinder(
            "Insulator",
            radius=0.045,
            depth=0.25,
            location=(
                x,
                0,
                8.0
            ),
            vertices=12,
            material=materials["metal"]
        )

        parts.append(insulator)

    result = join_objects(
        parts,
        "SM_SJL_Pole_Concrete_01"
    )

    set_origin_bottom(result)

    export_fbx(
        result,
        POLES
    )

    result.hide_set(True)