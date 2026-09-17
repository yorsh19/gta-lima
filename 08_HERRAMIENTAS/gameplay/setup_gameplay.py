"""Create the first playable sandbox and two data-driven car variants.
Existing Kalex assets/levels are preserved on repeat runs.
"""
import json
from pathlib import Path
import unreal as u

assets = u.AssetToolsHelpers.get_asset_tools()
editor = u.get_editor_subsystem(u.EditorActorSubsystem)
levels = u.get_editor_subsystem(u.LevelEditorSubsystem)
root = '/Game/KalexUrban/Gameplay'
report = {'created': [], 'preserved': []}

def blueprint(name, parent):
    path = root + '/' + name
    if u.EditorAssetLibrary.does_asset_exist(path):
        report['preserved'].append(path)
        return u.load_asset(path)
    factory = u.BlueprintFactory()
    factory.set_editor_property('parent_class', parent)
    bp = assets.create_asset(name, root, u.Blueprint, factory)
    if not bp:
        raise RuntimeError('Could not create ' + path)
    report['created'].append(path)
    return bp

source_class = u.EditorAssetLibrary.load_blueprint_class('/Game/VehicleTemplate/Blueprints/SportsCar/BP_VehicleAdvSportsCar')
source = u.get_default_object(source_class)
source_mesh = source.get_component_by_class(u.SkeletalMeshComponent)
source_move = source.get_component_by_class(u.ChaosWheeledVehicleMovementComponent)
report['template_static_components'] = [
    {'name': c.get_name(), 'mesh': str(c.get_editor_property('static_mesh')),
     'socket': str(c.get_attach_socket_name()), 'transform': str(c.get_relative_transform())}
    for c in source.get_components_by_class(u.StaticMeshComponent)]

for suffix, mass in [('Prototype', 1500.0), ('Heavy', 1900.0)]:
    name = 'DA_Vehicle_' + suffix
    path = root + '/' + name
    if u.EditorAssetLibrary.does_asset_exist(path):
        definition = u.load_asset(path)
        report['preserved'].append(path)
    else:
        factory = u.DataAssetFactory()
        factory.set_editor_property('data_asset_class', u.KalexVehicleDefinition)
        definition = assets.create_asset(name, root, u.KalexVehicleDefinition, factory)
        definition.set_editor_property('mesh', source_mesh.get_editor_property('skeletal_mesh_asset'))
        definition.set_editor_property('animation_class', source_mesh.get_editor_property('anim_class'))
        for dest, src in [('wheels','wheel_setups'), ('engine','engine_setup'),
                          ('transmission','transmission_setup'), ('differential','differential_setup'),
                          ('steering','steering_setup')]:
            definition.set_editor_property(dest, source_move.get_editor_property(src))
        definition.set_editor_property('mass_kg', mass)
        u.EditorAssetLibrary.save_loaded_asset(definition)
        report['created'].append(path)
    bp_path = root + '/BP_Vehicle_' + suffix
    exists = u.EditorAssetLibrary.does_asset_exist(bp_path)
    bp = blueprint('BP_Vehicle_' + suffix, u.KalexVehicleBase)
    if not exists:
        cdo = u.get_default_object(bp.generated_class())
        cdo.set_editor_property('definition', definition)
        u.BlueprintEditorLibrary.compile_blueprint(bp)
        u.EditorAssetLibrary.save_loaded_asset(bp)

gm = blueprint('BP_KalexGameMode', u.KalexGameMode)
u.EditorAssetLibrary.save_loaded_asset(gm)
map_path = '/Game/KalexUrban/Levels/L_GameplayFoundation'
if not u.EditorAssetLibrary.does_asset_exist(map_path):
    if not levels.new_level(map_path):
        raise RuntimeError('Could not create sandbox map')
    cube = u.load_asset('/Engine/BasicShapes/Cube')
    def block(label, position, scale):
        a = editor.spawn_actor_from_class(u.StaticMeshActor, u.Vector(*position))
        a.set_actor_label(label)
        c = a.static_mesh_component
        c.set_static_mesh(cube)
        c.set_collision_profile_name('BlockAll')
        a.set_actor_scale3d(u.Vector(*scale))
        return a
    block('TestGround', (0,0,-50), (200,100,1))
    block('CrashBarrier', (3500,0,100), (1,12,2))
    block('BoundaryLeft', (0,-2500,100), (200,1,2))
    block('BoundaryRight', (0,2500,100), (200,1,2))
    start = editor.spawn_actor_from_class(u.PlayerStart, u.Vector(0,-260,100), u.Rotator(0,90,0))
    start.set_editor_property('player_start_tag', 'KalexStart')
    for suffix, pos in [('Prototype',(0,0,80)), ('Heavy',(700,700,80))]:
        cls = u.EditorAssetLibrary.load_blueprint_class(root + '/BP_Vehicle_' + suffix)
        car = editor.spawn_actor_from_class(cls, u.Vector(*pos))
        car.set_actor_label('Kalex_' + suffix)
    sun = editor.spawn_actor_from_class(u.DirectionalLight, u.Vector(0,0,1000), u.Rotator(-45,30,0))
    editor.spawn_actor_from_class(u.SkyLight, u.Vector(0,0,600))
    editor.spawn_actor_from_class(u.SkyAtmosphere, u.Vector())
    world = u.get_editor_subsystem(u.UnrealEditorSubsystem).get_editor_world()
    world.get_world_settings().set_editor_property('default_game_mode', gm.generated_class())
    levels.save_current_level()
    report['created'].append(map_path)
else:
    report['preserved'].append(map_path)

out = Path(u.Paths.project_saved_dir()) / 'KalexValidation' / 'setup_gameplay.json'
out.parent.mkdir(parents=True, exist_ok=True)
out.write_text(json.dumps(report, indent=2), encoding='utf-8')
u.log('KALEX_GAMEPLAY_SETUP_OK')
