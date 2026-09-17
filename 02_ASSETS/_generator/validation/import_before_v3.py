import unreal
from pathlib import Path


# ============================================================
# PATHS
# ============================================================

SCRIPT_FILE = Path(__file__).resolve()

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
    options.import_materials = True
    options.import_textures = True

    options.static_mesh_import_data.combine_meshes = True

    options.static_mesh_import_data.generate_lightmap_u_vs = True

    options.static_mesh_import_data.auto_generate_collision = True

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

    return len(tasks)


# ============================================================
# STATIC MESH CONFIGURATION
# ============================================================

def configure_static_meshes():

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

    unreal.log("")
    unreal.log("=" * 60)
    unreal.log("KALEX URBAN - UE5 ASSET IMPORTER")
    unreal.log("=" * 60)

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

    configured = configure_static_meshes()

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


main()