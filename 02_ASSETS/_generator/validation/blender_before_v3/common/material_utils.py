import bpy


def create_material(
    name,
    color,
    roughness=0.7,
    metallic=0.0
):

    existing = bpy.data.materials.get(name)

    if existing:
        return existing

    material = bpy.data.materials.new(
        name=name
    )

    material.use_nodes = True

    bsdf = material.node_tree.nodes.get(
        "Principled BSDF"
    )

    if bsdf:

        bsdf.inputs[
            "Base Color"
        ].default_value = color

        bsdf.inputs[
            "Roughness"
        ].default_value = roughness

        bsdf.inputs[
            "Metallic"
        ].default_value = metallic

    return material


def create_default_materials():

    return {

        "brick": create_material(
            "M_SJL_Brick",
            (0.42, 0.12, 0.055, 1),
            0.9
        ),

        "concrete": create_material(
            "M_SJL_Concrete",
            (0.38, 0.38, 0.36, 1),
            0.95
        ),

        "plaster": create_material(
            "M_SJL_Plaster",
            (0.68, 0.62, 0.52, 1),
            0.85
        ),

        "blue": create_material(
            "M_SJL_BluePaint",
            (0.035, 0.20, 0.40, 1),
            0.75
        ),

        "orange": create_material(
            "M_SJL_OrangePaint",
            (0.65, 0.22, 0.07, 1),
            0.8
        ),

        "metal": create_material(
            "M_SJL_Metal",
            (0.08, 0.08, 0.075, 1),
            0.45,
            0.7
        ),

        "asphalt": create_material(
            "M_SJL_Asphalt",
            (0.065, 0.06, 0.055, 1),
            0.95
        ),

        "sidewalk": create_material(
            "M_SJL_Sidewalk",
            (0.34, 0.33, 0.30, 1),
            0.95
        ),

        "plastic": create_material(
            "M_SJL_Plastic",
            (0.03, 0.04, 0.05, 1),
            0.6
        )
    }