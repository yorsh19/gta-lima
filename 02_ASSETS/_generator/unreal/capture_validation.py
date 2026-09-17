"""Run via -ExecCmds='py .../capture_validation.py' so rendering has editor ticks."""
import time
from pathlib import Path
import unreal

OUT = Path(__file__).resolve().parents[1]/"validation"
level = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
assert level.load_level("/Game/KalexUrban/Levels/L_SJL_V3_Validation")
editor = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
position = unreal.Vector(1650,-3200,1700)
rotation = unreal.MathLibrary.find_look_at_rotation(position, unreal.Vector(1650,0,500))
editor.set_level_viewport_camera_info(position,rotation)
world = editor.get_editor_world()
actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
for actor in actors.get_all_level_actors():
    if isinstance(actor, unreal.DirectionalLight):
        actor.get_component_by_class(unreal.DirectionalLightComponent).set_editor_property("intensity", 50000.0)
    if isinstance(actor, unreal.LimaBuildingGenerator):
        unreal.log(f"[KALEX] Capture {actor.get_actor_label()}: {actor.get_actor_location()} components={len(actor.get_components_by_class(unreal.StaticMeshComponent))}")
if not any(isinstance(a,unreal.SkyAtmosphere) for a in actors.get_all_level_actors()):
    actors.spawn_actor_from_class(unreal.SkyAtmosphere, unreal.Vector())
if not any(a.get_actor_label() == "V3_Review_Ground" for a in actors.get_all_level_actors()):
    ground = actors.spawn_actor_from_class(unreal.StaticMeshActor,unreal.Vector(1650,0,-10))
    ground.static_mesh_component.set_static_mesh(unreal.EditorAssetLibrary.load_asset("/Engine/BasicShapes/Cube"))
    ground.static_mesh_component.set_material(0,unreal.EditorAssetLibrary.load_asset("/Game/KalexUrban/Materials/Instances/Concrete/M_SJL_Concrete"))
    ground.set_actor_scale3d(unreal.Vector(65,35,.2))
camera = actors.spawn_actor_from_class(unreal.CameraActor, position, rotation)
settings = camera.camera_component.get_editor_property("post_process_settings")
settings.set_editor_property("override_auto_exposure_min_brightness", True)
settings.set_editor_property("override_auto_exposure_max_brightness", True)
settings.set_editor_property("auto_exposure_min_brightness", 12.0)
settings.set_editor_property("auto_exposure_max_brightness", 12.0)
camera.camera_component.set_editor_property("post_process_settings", settings)
camera.camera_component.set_editor_property("post_process_blend_weight", 1.0)
# Review lighting only; the saved validation map is left intact.
unreal.SystemLibrary.execute_console_command(world, "r.EyeAdaptationQuality 2")
unreal.SystemLibrary.execute_console_command(world, "r.ScreenPercentage 100")
started = time.monotonic()
task = None


def tick(delta):
    global task
    elapsed = time.monotonic()-started
    if task is None and elapsed > 15:
        task = unreal.AutomationLibrary.take_high_res_screenshot(1800,1000,str(OUT/"ue_v3.png"), camera=camera)
    if (task is not None and task.is_task_done()) or elapsed > 90:
        unreal.unregister_slate_post_tick_callback(handle)
        unreal.SystemLibrary.quit_editor()


handle = unreal.register_slate_post_tick_callback(tick)
