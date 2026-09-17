import bpy
import math


def clear_scene():

    bpy.ops.object.select_all(
        action='SELECT'
    )

    bpy.ops.object.delete(
        use_global=False
    )


def deselect_all():

    bpy.ops.object.select_all(
        action='DESELECT'
    )


def apply_transforms(obj):

    deselect_all()

    obj.select_set(True)

    bpy.context.view_layer.objects.active = obj

    bpy.ops.object.transform_apply(
        location=False,
        rotation=True,
        scale=True
    )


def create_box(
    name,
    size,
    location=(0, 0, 0),
    material=None
):

    bpy.ops.mesh.primitive_cube_add(
        location=location
    )

    obj = bpy.context.object

    obj.name = name

    obj.dimensions = size

    apply_transforms(obj)

    if material:
        obj.data.materials.append(material)

    return obj


def create_cylinder(
    name,
    radius,
    depth,
    location=(0, 0, 0),
    vertices=16,
    material=None
):

    bpy.ops.mesh.primitive_cylinder_add(
        vertices=vertices,
        radius=radius,
        depth=depth,
        location=location
    )

    obj = bpy.context.object

    obj.name = name

    apply_transforms(obj)

    if material:
        obj.data.materials.append(material)

    return obj


def create_uv_sphere(
    name,
    radius,
    location=(0, 0, 0),
    material=None
):

    bpy.ops.mesh.primitive_uv_sphere_add(
        segments=16,
        ring_count=8,
        radius=radius,
        location=location
    )

    obj = bpy.context.object

    obj.name = name

    apply_transforms(obj)

    if material:
        obj.data.materials.append(material)

    return obj


def join_objects(objects, name):

    deselect_all()

    valid = [
        obj for obj in objects
        if obj is not None
    ]

    if not valid:
        return None

    for obj in valid:
        obj.select_set(True)

    bpy.context.view_layer.objects.active = valid[0]

    bpy.ops.object.join()

    result = bpy.context.object
    result.name = name

    return result


def bevel_object(
    obj,
    width=0.02,
    segments=2
):

    modifier = obj.modifiers.new(
        name="AutoBevel",
        type='BEVEL'
    )

    modifier.width = width
    modifier.segments = segments

    bpy.context.view_layer.objects.active = obj

    bpy.ops.object.modifier_apply(
        modifier=modifier.name
    )


def set_origin_bottom(obj):

    bpy.context.view_layer.objects.active = obj

    obj.select_set(True)

    min_z = min(
        vertex.co.z
        for vertex in obj.data.vertices
    )

    cursor = bpy.context.scene.cursor
    previous = cursor.location.copy()

    cursor.location = (
        obj.location.x,
        obj.location.y,
        obj.location.z + min_z
    )

    bpy.ops.object.origin_set(
        type='ORIGIN_CURSOR'
    )

    cursor.location = previous


def rotate_x(obj, degrees):

    obj.rotation_euler.x = math.radians(degrees)

    apply_transforms(obj)


def rotate_y(obj, degrees):

    obj.rotation_euler.y = math.radians(degrees)

    apply_transforms(obj)


def rotate_z(obj, degrees):

    obj.rotation_euler.z = math.radians(degrees)

    apply_transforms(obj)


def planar_uv_meters(obj):
    """UV0: one UV unit per metre, projected per face, consistent across modules."""
    uv = obj.data.uv_layers.active or obj.data.uv_layers.new(name="UVMap")
    for face in obj.data.polygons:
        axis = max(range(3), key=lambda i: abs(face.normal[i]))
        axes = ((1, 2), (0, 2), (0, 1))[axis]
        for loop_index in face.loop_indices:
            point = obj.matrix_world @ obj.data.vertices[obj.data.loops[loop_index].vertex_index].co
            uv.data[loop_index].uv = (point[axes[0]], point[axes[1]])


def set_origin_ground_center(obj):
    """New modular contract: X width, Y depth, Z up; origin (0,0,0) at ground."""
    deselect_all()
    obj.select_set(True)
    bpy.context.view_layer.objects.active = obj
    cursor = bpy.context.scene.cursor
    previous = cursor.location.copy()
    cursor.location = (0, 0, 0)
    bpy.ops.object.origin_set(type='ORIGIN_CURSOR')
    cursor.location = previous
