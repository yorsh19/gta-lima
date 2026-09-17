from common.mesh_utils import (
    create_box
)

from common.export_utils import export_fbx

from config import ROADS


def generate(materials):

    road = create_box(
        "SM_SJL_Road_Asphalt_2Lane",
        (
            10.0,
            7.0,
            0.10
        ),
        (
            5.0,
            0,
            -0.05
        ),
        materials["asphalt"]
    )

    export_fbx(
        road,
        ROADS
    )

    road.hide_set(True)

    narrow = create_box(
        "SM_SJL_Road_Asphalt_Narrow",
        (
            10.0,
            5.0,
            0.10
        ),
        (
            5.0,
            0,
            -0.05
        ),
        materials["asphalt"]
    )

    export_fbx(
        narrow,
        ROADS
    )

    narrow.hide_set(True)