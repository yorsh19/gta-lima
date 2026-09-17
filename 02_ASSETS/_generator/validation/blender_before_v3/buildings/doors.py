from common.mesh_utils import (
    create_box,
    join_objects,
    bevel_object
)

from common.export_utils import export_fbx
from common.mesh_utils import set_origin_bottom

from config import BUILDING_DOORS


def generate(materials):

    # -------------------------
    # PUERTA SIMPLE
    # -------------------------

    door = create_box(
        "Door",
        (
            0.95,
            0.08,
            2.10
        ),
        (
            0,
            0,
            1.05
        ),
        materials["metal"]
    )

    bevel_object(
        door,
        0.015,
        2
    )

    set_origin_bottom(door)

    door.name = "SM_SJL_Door_Metal_01"

    export_fbx(
        door,
        BUILDING_DOORS
    )

    door.hide_set(True)

    # -------------------------
    # PORTON
    # -------------------------

    parts = []

    gate = create_box(
        "Gate",
        (
            2.8,
            0.10,
            2.35
        ),
        (
            0,
            0,
            1.175
        ),
        materials["metal"]
    )

    parts.append(gate)

    for x in [
        -1.0,
        -0.5,
        0,
        0.5,
        1.0
    ]:

        bar = create_box(
            "GateDetail",
            (
                0.035,
                0.03,
                2.20
            ),
            (
                x,
                -0.065,
                1.15
            ),
            materials["concrete"]
        )

        parts.append(bar)

    result = join_objects(
        parts,
        "SM_SJL_GarageDoor_01"
    )

    set_origin_bottom(result)

    export_fbx(
        result,
        BUILDING_DOORS
    )

    result.hide_set(True)