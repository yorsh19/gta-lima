from common.mesh_utils import (
    create_cylinder,
    create_box,
    join_objects,
    rotate_y
)

from common.export_utils import export_fbx

from config import STREETLIGHTS


def generate(materials):

    parts = []

    pole = create_cylinder(
        "Pole",
        0.07,
        7.0,
        (
            0,
            0,
            3.5
        ),
        12,
        materials["metal"]
    )

    parts.append(pole)

    arm = create_cylinder(
        "Arm",
        0.04,
        1.4,
        (
            0.65,
            0,
            6.8
        ),
        12,
        materials["metal"]
    )

    rotate_y(
        arm,
        90
    )

    parts.append(arm)

    lamp = create_box(
        "Lamp",
        (
            0.50,
            0.25,
            0.12
        ),
        (
            1.35,
            0,
            6.8
        ),
        materials["metal"]
    )

    parts.append(lamp)

    result = join_objects(
        parts,
        "SM_SJL_StreetLight_01"
    )

    export_fbx(
        result,
        STREETLIGHTS
    )

    result.hide_set(True)