import unreal
import json
import sys
from pathlib import Path


# ============================================================
# PATHS
# ============================================================

SCRIPT_FILE = Path(__file__).resolve()
sys.path.insert(0, str(SCRIPT_FILE.parent))
from material_library import ensure_material_library

GENERATOR_ROOT = SCRIPT_FILE.parent.parent
ASSETS_ROOT = GENERATOR_ROOT.parent

SOURCE_3D = ASSETS_ROOT / "3D"


# ============================================================
# UE DESTINATIONS
# ============================================================

DESTINATIONS = {
    "edificios/modular":
        "/Game/KalexUrban/Environment/Buildings/Modular",

    "edificios/puertas":
        "/Game/KalexUrban/Environment/Buildings/Doors",

    "edificios/ventanas":
        "/Game/KalexUrban/Environment/Buildings/Windows",

    "edificios/techos":
        "/Game/KalexUrban/Environment/Buildings/Roofs",

    "edificios/props":
        "/Game/KalexUrban/Environment/Buildings/Props",

    "infraestructura/pistas":
        "/Game/KalexUrban/Environment/Roads",

    "infraestructura/veredas":
        "/Game/KalexUrban/Environment/Sidewalks",

    "infraestructura/postes":
        "/Game/KalexUrban/Environment/Infrastructure/Poles",

    "infraestructura/cables":
        "/Game/KalexUrban/Environment/Infrastructure/Cables",

    "infraestructura/alumbrado":
        "/Game/KalexUrban/Environment/Infrastructure/StreetLights",
}


# ============================================================
# IMPORT
# ============================================================

def create_import_task(filename, destination):

    task = unreal.AssetImportTask()

    task.filename = str(filename)

    task.destination_path = destination

    task.automated = True
    task.replace_existing = True
    task.save = True

    options = unreal.FbxImportUI()

    options.import_mesh = True
    options.import_as_skeletal = False
    options.import_materials = False
    options.import_textures = False

    options.static_mesh_import_data.combine_meshes = True

    options.static_mesh_import_data.generate_lightmap_u_vs = True

    options.static_mesh_import_data.auto_generate_collision = True
    options.static_mesh_import_data.one_convex_hull_per_ucx = True
    # Preserve the explicit ground origin of V3 modules.
    options.static_mesh_import_data.transform_vertex_to_absolute = True

    task.options = options

    return task


def import_directory(relative_source, destination):

    source = SOURCE_3D / relative_source

    if not source.exists():

        unreal.log_warning(
            f"[KALEX] Missing directory: {source}"
        )

        return 0

    files = sorted(source.glob("*.fbx"))

    if not files:

        unreal.log_warning(
            f"[KALEX] No FBX found: {source}"
        )

        return 0

    unreal.EditorAssetLibrary.make_directory(
        destination
    )

    tasks = []

    for file in files:

        unreal.log(
            f"[KALEX] Importing: {file.name}"
        )

        tasks.append(
            create_import_task(
                file,
                destination
            )
        )

    unreal.AssetToolsHelpers \
        .get_asset_tools() \
        .import_asset_tasks(tasks)

    imported = 0
    for task in tasks:
        meshes = [unreal.EditorAssetLibrary.load_asset(p) for p in task.imported_object_paths]
        meshes = [m for m in meshes if isinstance(m, unreal.StaticMesh)]
        if len(meshes) != 1:
            raise RuntimeError(f"Expected one mesh from {task.filename}, got {len(meshes)}")
        manifest_file = Path(task.filename).with_suffix(".materials.json")
        if manifest_file.exists():
            manifest = json.loads(manifest_file.read_text(encoding="utf-8"))
            actual = {str(slot.material_slot_name) for slot in meshes[0].static_materials}
            if actual != set(manifest["slots"]):
                raise RuntimeError(f"FBX slot mismatch: {task.filename}: {actual}")
            if manifest["custom_collision"]:
                subsystem = unreal.get_editor_subsystem(unreal.StaticMeshEditorSubsystem)
                if subsystem.get_convex_collision_count(meshes[0]) == 0:
                    raise RuntimeError(f"Missing UCX collision: {task.filename}")
        imported += 1
    return imported


# ============================================================
# STATIC MESH CONFIGURATION
# ============================================================

def configure_static_meshes(materials):

    registry = unreal.AssetRegistryHelpers \
        .get_asset_registry()

    assets = registry.get_assets_by_path(
        "/Game/KalexUrban/Environment",
        recursive=True
    )

    count = 0

    for asset_data in assets:

        asset = asset_data.get_asset()

        if not isinstance(
            asset,
            unreal.StaticMesh
        ):
            continue

        unreal.log(
            f"[KALEX] Configuring: {asset.get_name()}"
        )
        if not asset.get_name().startswith("SM_SJL_"):
            continue
        for index, slot in enumerate(asset.static_materials):
            slot_name = str(slot.material_slot_name)
            material = materials.get(slot_name)
            if material is None:
                raise RuntimeError(f"Unknown material slot {asset.get_name()}[{index}]: {slot_name}")
            asset.set_material(index, material)

        # Nanite
        try:

            nanite_settings = asset.get_editor_property(
                "nanite_settings"
            )

            nanite_settings.enabled = True

            asset.set_editor_property(
                "nanite_settings",
                nanite_settings
            )

        except Exception as error:

            unreal.log_warning(
                f"[KALEX] Nanite skipped "
                f"{asset.get_name()}: {error}"
            )

        asset.modify()

        unreal.EditorAssetLibrary.save_loaded_asset(
            asset
        )

        count += 1

    return count


# ============================================================
# MAIN
# ============================================================

def main():
    marker = GENERATOR_ROOT / "validation" / "import_success.json"
    marker.parent.mkdir(parents=True, exist_ok=True)
    marker.unlink(missing_ok=True)

    unreal.log("")
    unreal.log("=" * 60)
    unreal.log("KALEX URBAN - UE5 ASSET IMPORTER")
    unreal.log("=" * 60)

    materials = ensure_material_library()
    total = 0

    for source, destination in DESTINATIONS.items():

        unreal.log("")
        unreal.log(
            f"[KALEX] {source}"
        )

        total += import_directory(
            source,
            destination
        )

    unreal.log("")
    unreal.log(
        f"[KALEX] Imported FBX: {total}"
    )

    configured = configure_static_meshes(materials)

    unreal.log(
        f"[KALEX] Configured StaticMeshes: {configured}"
    )

    unreal.EditorAssetLibrary.save_directory(
        "/Game/KalexUrban",
        only_if_is_dirty=False,
        recursive=True
    )

    unreal.log("")
    unreal.log("=" * 60)
    unreal.log("KALEX URBAN IMPORT COMPLETE")
    unreal.log("=" * 60)
    marker.write_text(json.dumps({"imported":total, "configured":configured}), encoding="utf-8")


main()
