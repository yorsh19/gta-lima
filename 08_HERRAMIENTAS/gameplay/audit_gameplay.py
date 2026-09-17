"""Read-only Unreal asset audit. Run with UnrealEditor-Cmd -run=pythonscript."""
import json
from pathlib import Path
import unreal as u

report = {}
for name, path in {
    'character': '/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter',
    'game_mode': '/Game/ThirdPerson/Blueprints/BP_ThirdPersonGameMode',
    'template_car': '/Game/VehicleTemplate/Blueprints/SportsCar/BP_VehicleAdvSportsCar',
}.items():
    cls = u.EditorAssetLibrary.load_blueprint_class(path)
    if not cls:
        raise RuntimeError('Missing class: ' + path)
    cdo = u.get_default_object(cls)
    report[name] = {'class': cls.get_path_name()}
    if name == 'game_mode':
        for prop in ['default_pawn_class', 'player_controller_class']:
            report[name][prop] = str(cdo.get_editor_property(prop))
    if name == 'template_car':
        mesh = cdo.get_component_by_class(u.SkeletalMeshComponent)
        move = cdo.get_component_by_class(u.ChaosWheeledVehicleMovementComponent)
        report[name]['mesh'] = str(mesh.get_editor_property('skeletal_mesh_asset'))
        report[name]['animation'] = str(mesh.get_editor_property('anim_class'))
        for prop in ['wheel_setups', 'engine_setup', 'transmission_setup', 'differential_setup', 'steering_setup', 'mass']:
            report[name][prop] = str(move.get_editor_property(prop))
out = Path(u.Paths.project_saved_dir()) / 'KalexValidation' / 'asset_audit.json'
out.parent.mkdir(parents=True, exist_ok=True)
out.write_text(json.dumps(report, indent=2), encoding='utf-8')
u.log('KALEX_ASSET_AUDIT_OK ' + str(out))
