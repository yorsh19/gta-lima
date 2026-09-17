"""Cheap Blender previews. FBX carries slots; Unreal builds its own materials."""
import json
from pathlib import Path
import bpy


def create_material(name, color, roughness=0.7, metallic=0.0):
    material = bpy.data.materials.get(name) or bpy.data.materials.new(name=name)
    material.use_nodes = True
    material.diffuse_color = color
    bsdf = next((n for n in material.node_tree.nodes if n.type == "BSDF_PRINCIPLED"), None)
    if bsdf is None:
        bsdf = material.node_tree.nodes.new("ShaderNodeBsdfPrincipled")
        output = material.node_tree.nodes.new("ShaderNodeOutputMaterial")
        material.node_tree.links.new(bsdf.outputs["BSDF"], output.inputs["Surface"])
    bsdf.inputs["Base Color"].default_value = color
    bsdf.inputs["Roughness"].default_value = roughness
    bsdf.inputs["Metallic"].default_value = metallic
    return material


def create_default_materials():
    catalog = json.loads((Path(__file__).resolve().parents[2] / "materials.json").read_text())
    result = {}
    for key, spec in catalog.items():
        mat = create_material(spec["name"], spec["color"], spec["roughness"], spec["metallic"])
        nodes, links = mat.node_tree.nodes, mat.node_tree.links
        bsdf = next(n for n in nodes if n.type == "BSDF_PRINCIPLED")
        uv = nodes.new("ShaderNodeTexCoord")
        color = spec["color"]
        if spec["family"] in ("Brick", "Tile"):
            tex = nodes.new("ShaderNodeTexBrick")
            tile = spec["family"] == "Tile"
            tex.offset = 0.0 if tile else 0.5
            tex.inputs["Scale"].default_value = 1.0
            tex.inputs["Brick Width"].default_value = 0.30 if tile else 0.24
            tex.inputs["Row Height"].default_value = 0.30 if tile else 0.09
            tex.inputs["Mortar Size"].default_value = 0.004 if tile else 0.008
            tex.inputs["Mortar"].default_value = (0.22, 0.21, 0.19, 1)
            tex.inputs["Color1"].default_value = color
            tex.inputs["Color2"].default_value = tuple(c * 0.86 for c in color[:3]) + (1,)
            links.new(uv.outputs["UV"], tex.inputs["Vector"])
            links.new(tex.outputs["Color"], bsdf.inputs["Base Color"])
        elif spec["family"] != "Glass":
            noise = nodes.new("ShaderNodeTexNoise")
            noise.inputs["Scale"].default_value = 18.0
            ramp = nodes.new("ShaderNodeValToRGB")
            ramp.color_ramp.elements[0].color = tuple(c * 0.85 for c in color[:3]) + (1,)
            ramp.color_ramp.elements[1].color = color
            links.new(uv.outputs["UV"], noise.inputs["Vector"])
            links.new(noise.outputs["Fac"], ramp.inputs["Fac"])
            links.new(ramp.outputs["Color"], bsdf.inputs["Base Color"])
        # Opaque dark glass proxy: inexpensive, compatible with Nanite.
        result[key] = mat
    result["brick"] = result["brick_red"]
    return result
