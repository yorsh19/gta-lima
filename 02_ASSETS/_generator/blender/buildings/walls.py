from common.mesh_utils import (
    create_box,
    bevel_object
)

from common.export_utils import export_fbx

from config import BUILDING_MODULAR


def generate(materials):

    definitions = [

        (
            "SM_SJL_Wall_Brick_300",
            3.0,
            materials["brick"]
        ),

        (
            "SM_SJL_Wall_Brick_400",
            4.0,
            materials["brick"]
        ),

        (
            "SM_SJL_Wall_Plaster_300",
            3.0,
            materials["plaster"]
        ),

        (
            "SM_SJL_Wall_Blue_300",
            3.0,
            materials["blue"]
        ),

        (
            "SM_SJL_Wall_Orange_300",
            3.0,
            materials["orange"]
        )
    ]

    for name, width, material in definitions:

        obj = create_box(
            name,
            (
                width,
                0.20,
                2.80
            ),
            (
                0,
                0,
                1.40
            ),
            material
        )

        bevel_object(
            obj,
            0.01,
            1
        )

        export_fbx(
            obj,
            BUILDING_MODULAR
        )

        obj.hide_set(True)