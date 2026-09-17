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

from config import BUILDING_PROPS


def generate(materials):

    # tanque de agua
    tank = create_cylinder(
        "SM_SJL_WaterTank_01",
        0.55,
        1.15,
        (
            0,
            0,
            0.575
        ),
        24,
        materials["plastic"]
    )

    export_fbx(
        tank,
        BUILDING_PROPS
    )

    tank.hide_set(True)
    set_origin_bottom(tank)

    # ladrillos apilados
    bricks = []

    for row in range(4):

        for column in range(5):

            x = column * 0.25
            z = row * 0.13

            brick = create_box(
                "Brick",
                (
                    0.24,
                    0.12,
                    0.10
                ),
                (
                    x,
                    0,
                    z + 0.05
                ),
                materials["brick"]
            )

            bricks.append(brick)

    stack = join_objects(
        bricks,
        "SM_SJL_BrickStack_01"
    )
    set_origin_bottom(stack)

    export_fbx(
        stack,
        BUILDING_PROPS
    )

    stack.hide_set(True)