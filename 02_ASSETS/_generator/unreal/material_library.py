"""Native UE5 MVP library. Existing artist-edited materials are never rebuilt."""
import json
from pathlib import Path
import unreal

ROOT = "/Game/KalexUrban/Materials"
LIB = unreal.MaterialEditingLibrary


def create_master(name):
    path = ROOT + "/Master"
    existing = unreal.EditorAssetLibrary.load_asset(path + "/" + name) if unreal.EditorAssetLibrary.does_asset_exist(path + "/" + name) else None
    if existing:
        return existing
    mat = unreal.AssetToolsHelpers.get_asset_tools().create_asset(name, path, unreal.Material, unreal.MaterialFactoryNew())
    if not mat:
        raise RuntimeError(f"Cannot create {name}")
    def parameter(label, value, prop=None):
        vector = isinstance(value, tuple)
        node = LIB.create_material_expression(mat, unreal.MaterialExpressionVectorParameter if vector else unreal.MaterialExpressionScalarParameter)
        node.set_editor_property("parameter_name", label)
        node.set_editor_property("default_value", unreal.LinearColor(*value) if vector else value)
        if prop is not None:
            assert LIB.connect_material_property(node, "", prop)
        return node
    color = parameter("BaseColor", (.38,.38,.36,1))
    parameter("Roughness", .8, unreal.MaterialProperty.MP_ROUGHNESS)
    parameter("Metallic", 0.0, unreal.MaterialProperty.MP_METALLIC)
    if name == "M_Master_Building":
        pattern = parameter("Pattern", 0.0)
        uv = LIB.create_material_expression(mat, unreal.MaterialExpressionTextureCoordinate)
        custom = LIB.create_material_expression(mat, unreal.MaterialExpressionCustom)
        custom.set_editor_property("output_type", unreal.CustomMaterialOutputType.CMOT_FLOAT3)
        inputs = []
        for pin_name in ("UV", "Tint", "Pattern"):
            pin = unreal.CustomInput()
            pin.set_editor_property("input_name", pin_name)
            inputs.append(pin)
        custom.set_editor_property("inputs", inputs)
        custom.set_editor_property("code", """
float2 size = Pattern > 1.5 ? float2(.30,.30) : float2(.24,.09);
float2 p = UV / size;
if (Pattern < 1.5) p.x += fmod(floor(p.y),2.0)*.5;
float2 edge = min(frac(p), 1.0-frac(p))*size;
float mortar = Pattern > 1.5 ? .004 : .008;
float aa = max(fwidth(UV.x), fwidth(UV.y));
float body = smoothstep(mortar, mortar+max(aa,.001),min(edge.x,edge.y));
return Pattern < .5 ? Tint : lerp(float3(.22,.21,.19),Tint,body);
""")
        for source, pin in ((uv,"UV"),(color,"Tint"),(pattern,"Pattern")):
            assert LIB.connect_material_expressions(source, "", custom, pin)
        assert LIB.connect_material_property(custom, "", unreal.MaterialProperty.MP_BASE_COLOR)
    else:
        assert LIB.connect_material_property(color, "", unreal.MaterialProperty.MP_BASE_COLOR)
    if name == "M_Master_Decal":
        mat.set_editor_property("blend_mode", unreal.BlendMode.BLEND_TRANSLUCENT)
        mat.set_editor_property("material_domain", unreal.MaterialDomain.MD_DEFERRED_DECAL)
        parameter("Opacity", 1.0, unreal.MaterialProperty.MP_OPACITY)
    LIB.layout_material_expressions(mat)
    LIB.recompile_material(mat)
    unreal.EditorAssetLibrary.save_loaded_asset(mat)
    return mat


def ensure_material_library():
    catalog = json.loads((Path(__file__).resolve().parents[1]/"materials.json").read_text())
    for family in ("Brick","Concrete","Plaster","Paint","Tile","Metal","Glass"):
        unreal.EditorAssetLibrary.make_directory(ROOT+"/Instances/"+family)
    for family in ("Brick","Concrete","Plaster","Tile","Metal"):
        unreal.EditorAssetLibrary.make_directory(ROOT+"/Textures/"+family)
    unreal.EditorAssetLibrary.make_directory(ROOT+"/Master")
    masters = {kind:create_master("M_Master_"+kind) for kind in ("Surface","Building","Glass","Decal")}
    result = {}
    for spec in catalog.values():
        name, family = spec["name"], spec["family"]
        directory = ROOT+"/Instances/"+family
        path = directory+"/"+name
        if unreal.EditorAssetLibrary.does_asset_exist(path):
            instance = unreal.EditorAssetLibrary.load_asset(path)
        else:
            instance = unreal.AssetToolsHelpers.get_asset_tools().create_asset(name, directory, unreal.MaterialInstanceConstant, unreal.MaterialInstanceConstantFactoryNew())
            kind = "Glass" if family == "Glass" else "Building" if family in ("Brick","Tile","Plaster","Paint") else "Surface"
            LIB.set_material_instance_parent(instance, masters[kind])
            LIB.set_material_instance_vector_parameter_value(instance, "BaseColor", unreal.LinearColor(*spec["color"]))
            for parameter in ("roughness","metallic"):
                LIB.set_material_instance_scalar_parameter_value(instance, parameter.capitalize(), spec[parameter])
            if kind == "Building":
                LIB.set_material_instance_scalar_parameter_value(instance, "Pattern", 1.0 if family == "Brick" else 2.0 if family == "Tile" else 0.0)
            # UE 5.6 setters return false even after success; verify the stored values.
            actual = LIB.get_material_instance_vector_parameter_value(instance, "BaseColor")
            assert all(abs(getattr(actual,c)-v)<1e-5 for c,v in zip(("r","g","b","a"), spec["color"])), name
            for parameter in ("roughness", "metallic"):
                assert abs(LIB.get_material_instance_scalar_parameter_value(instance, parameter.capitalize())-spec[parameter])<1e-5, name
            LIB.update_material_instance(instance)
            unreal.EditorAssetLibrary.save_loaded_asset(instance)
        result[name] = instance
    for old, new in {"M_SJL_Brick":"M_SJL_Brick_Red","M_SJL_BluePaint":"M_SJL_Paint_Blue","M_SJL_OrangePaint":"M_SJL_Paint_Orange"}.items():
        result[old] = result[new]
    return result
