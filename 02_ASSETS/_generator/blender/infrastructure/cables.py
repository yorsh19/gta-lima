from common.mesh_utils import (
    create_cylinder,
    rotate_y
)

from common.export_utils import export_fbx

from config import CABLES


def generate(materials):

    cable = create_cylinder(
        "SM_SJL_Cable_Segment",
        radius=0.008,
        depth=1.0,
        location=(
            0,
            0,
            0
        ),
        vertices=8,
        material=materials["plastic"]
    )

    # Los spline meshes del proyecto usan X
    rotate_y(
        cable,
        90
    )

    export_fbx(
        cable,
        CABLES
    )

    cable.hide_set(True)