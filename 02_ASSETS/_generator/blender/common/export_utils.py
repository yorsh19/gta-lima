import bpy
import json
from pathlib import Path

from common.mesh_utils import deselect_all, planar_uv_meters


def export_fbx(
    obj,
    output_directory,
    filename=None,
    collision_objects=None
):

    if obj is None:
        return

    output_directory = Path(
        output_directory
    )

    output_directory.mkdir(
        parents=True,
        exist_ok=True
    )

    if filename is None:
        filename = obj.name

    output_file = (
        output_directory /
        f"{filename}.fbx"
    )

    deselect_all()

    obj.select_set(True)
    planar_uv_meters(obj)
    for collision in collision_objects or []:
        collision.select_set(True)

    bpy.context.view_layer.objects.active = obj

    bpy.ops.export_scene.fbx(
        filepath=str(output_file),
        use_selection=True,
        object_types={'MESH'},
        apply_unit_scale=True,
        apply_scale_options='FBX_SCALE_ALL',
        axis_forward='-Y',
        axis_up='Z',
        add_leaf_bones=False
    )

    print(
        f"[EXPORTED] {output_file}"
    )
    output_file.with_suffix(".materials.json").write_text(json.dumps({
        "schema": 1,
        "mesh": filename,
        "slots": [slot.material.name for slot in obj.material_slots],
        "custom_collision": bool(collision_objects),
    }, indent=2), encoding="utf-8")
