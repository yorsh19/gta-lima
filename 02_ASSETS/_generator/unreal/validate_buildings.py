"""Integration checks in the actual UE editor, plus a saved four-seed review level."""
import json
from pathlib import Path
import unreal

OUT = Path(__file__).resolve().parents[1] / "validation"
actor_system = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
level_system = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
mesh_system = unreal.get_editor_subsystem(unreal.StaticMeshEditorSubsystem)
ROOT = "/Game/KalexUrban/Environment/Buildings/Modular/"


def signature(actor):
    rows = []
    for c in actor.get_components_by_class(unreal.StaticMeshComponent):
        if "Generated" not in [str(t) for t in c.component_tags]:
            continue
        assert c.static_mesh is not None
        assert c.get_attach_parent() == actor.root_component, c.get_name()
        local = c.get_editor_property("relative_location")
        expected = unreal.MathLibrary.transform_location(actor.get_actor_transform(), local)
        assert (c.get_world_location()-expected).length()<.01, c.get_name()
        materials = [c.get_material(i) for i in range(c.get_num_materials())]
        assert all(m and "/KalexUrban/Materials/Instances/" in m.get_path_name() for m in materials)
        rows.append((c.static_mesh.get_name(), tuple(getattr(c.get_editor_property("relative_location"),a) for a in ("x","y","z")),
                     tuple(getattr(c.get_editor_property("relative_rotation"),a) for a in ("pitch","yaw","roll")), tuple(getattr(c.get_editor_property("relative_scale3d"),a) for a in ("x","y","z")),
                     tuple(m.get_path_name() for m in materials)))
    return sorted(rows)


def main():
    marker = OUT/"building_validation.json"
    marker.unlink(missing_ok=True)
    # Validate import units, origin and preservation of custom collision.
    dimensions = {}
    for kind, widths in (("Wall",(50,100,150,200)),("Window",(100,120,150)),("Door",(90,100)),("Garage",(250,300))):
        for width in widths:
            name = f"SM_SJL_Facade_{kind}_{width:03d}"
            mesh = unreal.EditorAssetLibrary.load_asset(ROOT+name)
            assert mesh, name
            bounds = mesh.get_bounding_box()
            size = bounds.max-bounds.min
            assert abs(size.x-width)<.1 and abs(size.z-280)<.1, (name,str(size))
            assert abs(bounds.min.z)<.1, (name,str(bounds.min))
            assert mesh_system.get_convex_collision_count(mesh)>0, name
            dimensions[name] = str(size)
    # Test in a transient world; never edit an existing map.
    world = unreal.EditorLoadingAndSavingUtils.new_blank_map(False)
    assert world
    actor = actor_system.spawn_actor_from_class(unreal.LimaBuildingGenerator, unreal.Vector())
    actor.set_editor_property("generate_on_construction", False)
    cases = 0
    for width in (300, 450, 475, 600, 925, 2000):
        for windows in (0, 2, 5):
            for garage in (False, True):
                actor.set_editor_property("building_width", width)
                actor.set_editor_property("windows_per_floor", windows)
                actor.set_editor_property("has_garage", garage)
                actor.generate_building()
                first = signature(actor)
                actor.generate_building()
                assert first and first == signature(actor), (width,windows,garage)
                # Facade spans meet exactly; window and door modules have no wall behind them.
                for floor in (0,1):
                    spans = []
                    for c in actor.get_components_by_class(unreal.StaticMeshComponent):
                        loc = c.get_editor_property("relative_location")
                        if abs(loc.y+actor.get_editor_property("building_depth")/2)>.1 or abs(loc.z-floor*280)>.1:
                            continue
                        if not c.static_mesh.get_name().startswith("SM_SJL_Facade_"):
                            continue
                        b = c.static_mesh.get_bounding_box()
                        scale = c.get_editor_property("relative_scale3d")
                        spans.append((loc.x+b.min.x*scale.x,loc.x+b.max.x*scale.x))
                    spans.sort()
                    assert spans and abs(spans[0][0]+width/2)<.1 and abs(spans[-1][1]-width/2)<.1, (width,spans)
                    assert all(abs(a[1]-b[0])<.1 for a,b in zip(spans,spans[1:])), (width,spans)
                cases += 1
    # Explicit finish selection and per-floor overrides must leave glass/metal alone.
    actor.set_editor_property("building_width",600)
    actor.set_editor_property("windows_per_floor",2)
    actor.set_editor_property("vary_finish_by_seed",False)
    actor.set_editor_property("facade_type",unreal.LimaFacadeType.PAINTED_BLUE)
    override = unreal.EditorAssetLibrary.load_asset("/Game/KalexUrban/Materials/Instances/Tile/M_SJL_Tile_Beige")
    actor.set_editor_property("floor_material_overrides",[override])
    actor.generate_building()
    for c in actor.get_components_by_class(unreal.StaticMeshComponent):
        if not c.static_mesh.get_name().startswith("SM_SJL_Facade_"):
            continue
        floor = round(c.get_editor_property("relative_location").z/280)
        for i, slot in enumerate(c.static_mesh.static_materials):
            name = str(slot.material_slot_name)
            expected = "M_SJL_Tile_Beige" if floor == 0 else "M_SJL_Paint_Blue"
            assert c.get_material(i).get_name() == (expected if name == "M_SJL_Brick_Red" else name)
    actor_system.destroy_actor(actor)
    seeds = {}
    for i, seed in enumerate((100,101,102,103)):
        actor = actor_system.spawn_actor_from_class(unreal.LimaBuildingGenerator, unreal.Vector(i*1100,0,0))
        actor.set_editor_property("seed",seed)
        actor.randomize_building()
        actor.set_actor_label(f"SJL_Seed_{seed}")
        first = signature(actor)
        actor.generate_building()
        assert first == signature(actor), seed
        seeds[seed] = first
    assert len({str(s) for s in seeds.values()}) == 4
    # Create the requested blueprint only if it doesn't exist; preserve existing blueprints.
    bp_path = "/Game/KalexUrban/Procedural/BP_LimaBuildingGenerator"
    if not unreal.EditorAssetLibrary.does_asset_exist(bp_path):
        factory = unreal.BlueprintFactory()
        factory.set_editor_property("parent_class", unreal.LimaBuildingGenerator)
        bp = unreal.AssetToolsHelpers.get_asset_tools().create_asset("BP_LimaBuildingGenerator",
            "/Game/KalexUrban/Procedural",unreal.Blueprint,factory)
        assert bp
        unreal.EditorAssetLibrary.save_loaded_asset(bp)
    light = actor_system.spawn_actor_from_class(unreal.DirectionalLight,unreal.Vector(0,0,1500),unreal.Rotator(-45,-35,0))
    light.set_actor_label("V3_Review_Sun")
    light.get_component_by_class(unreal.DirectionalLightComponent).set_editor_property("intensity",50000.0)
    actor_system.spawn_actor_from_class(unreal.SkyAtmosphere,unreal.Vector())
    actor_system.spawn_actor_from_class(unreal.SkyLight,unreal.Vector(0,0,1500))
    ground = actor_system.spawn_actor_from_class(unreal.StaticMeshActor,unreal.Vector(1650,0,-10))
    ground.set_actor_label("V3_Review_Ground")
    ground.static_mesh_component.set_static_mesh(unreal.EditorAssetLibrary.load_asset("/Engine/BasicShapes/Cube"))
    ground.static_mesh_component.set_material(0,unreal.EditorAssetLibrary.load_asset("/Game/KalexUrban/Materials/Instances/Concrete/M_SJL_Concrete"))
    ground.set_actor_scale3d(unreal.Vector(65,35,.2))
    exposure = actor_system.spawn_actor_from_class(unreal.PostProcessVolume,unreal.Vector())
    exposure.set_editor_property("unbound",True)
    settings = exposure.get_editor_property("settings")
    for key in ("min_brightness","max_brightness"):
        settings.set_editor_property("override_auto_exposure_"+key,True)
        settings.set_editor_property("auto_exposure_"+key,12.0)
    exposure.set_editor_property("settings",settings)
    unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).set_level_viewport_camera_info(unreal.Vector(1600,-3600,1900),unreal.Rotator(-18,90,0))
    assert unreal.EditorLoadingAndSavingUtils.save_map(world, "/Game/KalexUrban/Levels/L_SJL_V3_Validation")
    marker.write_text(json.dumps({"cases":cases,"seeds":seeds,"dimensions":dimensions},indent=2),encoding="utf-8")
    unreal.log(f"[KALEX] V3 VALIDATION PASSED: {cases} layouts, seeds 100-103")


main()
