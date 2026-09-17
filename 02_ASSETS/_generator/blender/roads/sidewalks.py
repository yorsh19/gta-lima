from common.mesh_utils import create_box

from common.export_utils import export_fbx

from config import SIDEWALKS


def generate(materials):

    sidewalk = create_box(
        "SM_SJL_Sidewalk_100",
        (
            5.0,
            1.0,
            0.15
        ),
        (
            2.5,
            0,
            0.075
        ),
        materials["sidewalk"]
    )

    export_fbx(
        sidewalk,
        SIDEWALKS
    )

    sidewalk.hide_set(True)

    wide = create_box(
        "SM_SJL_Sidewalk_150",
        (
            5.0,
            1.5,
            0.15
        ),
        (
            2.5,
            0,
            0.075
        ),
        materials["sidewalk"]
    )

    export_fbx(
        wide,
        SIDEWALKS
    )

    wide.hide_set(True)