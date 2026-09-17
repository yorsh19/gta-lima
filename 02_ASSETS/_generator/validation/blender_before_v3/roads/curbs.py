from common.mesh_utils import create_box

from common.export_utils import export_fbx

from config import SIDEWALKS


def generate(materials):

    curb = create_box(
        "SM_SJL_Curb_500",
        (
            5.0,
            0.18,
            0.25
        ),
        (
            2.5,
            0,
            0.125
        ),
        materials["concrete"]
    )

    export_fbx(
        curb,
        SIDEWALKS
    )

    curb.hide_set(True)