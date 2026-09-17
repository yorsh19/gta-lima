import math
import random
import unreal


# -----------------------------------------------------------------------------
# Kalex Urban - Avenue blockout generator
# Creates a first-pass terrain + road layout inspired by Av. San Martin.
# Run from Unreal Python console:
#   import build_kalex_avenue_blockout as kalex
#   kalex.build_scene(clear_existing=True)
# -----------------------------------------------------------------------------


ROOT_TAG = "KalexUrban_Blockout"

# San Martin SJL tuned dimensions (meters)
AVENUE_LENGTH_M = 420.0
CARRIAGEWAY_WIDTH_M = 7.2   # 2 lanes per side (tight urban)
MEDIAN_WIDTH_M = 1.0
SIDEWALK_WIDTH_M = 2.2
LEFT_SETBACK_M = 1.4
RIGHT_SETBACK_M = 1.8
ROAD_GRADE_M_PER_M = 0.016
GRADE_ORIGIN_X_CM = (AVENUE_LENGTH_M * 100.0) * 0.5


def _log(message):
    unreal.log("[KalexUrban] {}".format(message))


def _warn(message):
    unreal.log_warning("[KalexUrban] {}".format(message))


def _asset(path):
    asset = unreal.EditorAssetLibrary.load_asset(path)
    if not asset:
        raise RuntimeError("Missing asset: {}".format(path))
    return asset


def _first_asset(paths):
    for path in paths:
        asset = unreal.EditorAssetLibrary.load_asset(path)
        if asset:
            return asset
    return None


def _pick_material(paths, fallback, name):
    material = _first_asset(paths)
    if not material:
        _warn("Material '{}' not found, using fallback.".format(name))
        return fallback
    return material


def _ensure_mi(asset_name, parent_material, base_color):
    folder = "/Game/KalexUrban/Materials"
    asset_path = "{}/{}".format(folder, asset_name)

    mi = unreal.EditorAssetLibrary.load_asset(asset_path) if unreal.EditorAssetLibrary.does_asset_exist(asset_path) else None
    if not mi:
        if not unreal.EditorAssetLibrary.does_directory_exist(folder):
            unreal.EditorAssetLibrary.make_directory(folder)
        factory = unreal.MaterialInstanceConstantFactoryNew()
        mi = unreal.AssetToolsHelpers.get_asset_tools().create_asset(asset_name, folder, unreal.MaterialInstanceConstant, factory)

    if not mi:
        return None

    try:
        mi.set_editor_property("parent", parent_material)
    except Exception:
        pass

    for p in ["Color", "BaseColor", "Tint", "AlbedoTint"]:
        try:
            unreal.MaterialEditingLibrary.set_material_instance_vector_parameter_value(mi, p, base_color)
        except Exception:
            pass

    unreal.EditorAssetLibrary.save_asset(asset_path)
    return mi


def _ensure_textured_road_material(texture_asset_path):
    tex = unreal.EditorAssetLibrary.load_asset(texture_asset_path)
    if not tex:
        _warn("Road texture not found: {}".format(texture_asset_path))
        return None

    folder = "/Game/KalexUrban/Materials"
    name = "M_KU_Road_Asphalt_Textured"
    material_path = "{}/{}".format(folder, name)

    mat = unreal.EditorAssetLibrary.load_asset(material_path) if unreal.EditorAssetLibrary.does_asset_exist(material_path) else None
    if not mat:
        if not unreal.EditorAssetLibrary.does_directory_exist(folder):
            unreal.EditorAssetLibrary.make_directory(folder)
        mat = unreal.AssetToolsHelpers.get_asset_tools().create_asset(name, folder, unreal.Material, unreal.MaterialFactoryNew())

    if not mat:
        return None

    try:
        unreal.MaterialEditingLibrary.delete_all_material_expressions(mat)

        tex_coord = unreal.MaterialEditingLibrary.create_material_expression(mat, unreal.MaterialExpressionTextureCoordinate, -600, 0)
        tex_coord.set_editor_property("u_tiling", 6.0)
        tex_coord.set_editor_property("v_tiling", 6.0)

        tex_sample = unreal.MaterialEditingLibrary.create_material_expression(mat, unreal.MaterialExpressionTextureSample, -350, 0)
        tex_sample.set_editor_property("texture", tex)

        rough = unreal.MaterialEditingLibrary.create_material_expression(mat, unreal.MaterialExpressionConstant, -350, 260)
        rough.set_editor_property("r", 0.86)

        spec = unreal.MaterialEditingLibrary.create_material_expression(mat, unreal.MaterialExpressionConstant, -350, 340)
        spec.set_editor_property("r", 0.04)

        unreal.MaterialEditingLibrary.connect_material_expressions(tex_coord, "", tex_sample, "UVs")
        unreal.MaterialEditingLibrary.connect_material_property(tex_sample, "RGB", unreal.MaterialProperty.MP_BASE_COLOR)
        unreal.MaterialEditingLibrary.connect_material_property(rough, "", unreal.MaterialProperty.MP_ROUGHNESS)
        unreal.MaterialEditingLibrary.connect_material_property(spec, "", unreal.MaterialProperty.MP_SPECULAR)

        unreal.MaterialEditingLibrary.recompile_material(mat)
        unreal.EditorAssetLibrary.save_asset(material_path)
        return mat
    except Exception as exc:
        _warn("Failed to build textured road material: {}".format(exc))
        return None


def _spawn_static_mesh(mesh, location, scale, rotation=unreal.Rotator(0.0, 0.0, 0.0), label=None):
    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.StaticMeshActor, location, rotation)
    actor.set_actor_scale3d(scale)
    actor.tags = list(set(list(actor.tags) + [ROOT_TAG]))
    if label:
        actor.set_actor_label(label)

    smc = actor.static_mesh_component
    smc.set_editor_property("static_mesh", mesh)
    smc.set_collision_profile_name("BlockAll")
    return actor


def _set_material(actor, material):
    smc = actor.static_mesh_component
    smc.set_material(0, material)


def _set_color_if_possible(actor, parent_material, color):
    smc = actor.static_mesh_component
    for param in ["Color", "BaseColor", "Tint", "AlbedoTint"]:
        try:
            smc.set_vector_parameter_value_on_materials(param, color)
            return
        except Exception:
            pass

    if not parent_material:
        return
    try:
        mid = unreal.MaterialInstanceDynamic.create(parent_material, actor)
        for param in ["Color", "BaseColor", "Tint", "AlbedoTint"]:
            try:
                mid.set_vector_parameter_value(param, color)
            except Exception:
                pass
        actor.static_mesh_component.set_material(0, mid)
    except Exception:
        pass


def clear_previous_blockout():
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    actors = actor_subsystem.get_all_level_actors()
    to_delete = []
    for actor in actors:
        tags = [str(t) for t in actor.tags]
        if ROOT_TAG in tags:
            to_delete.append(actor)

    if not to_delete:
        _log("No previous blockout actors found.")
        return

    for actor in to_delete:
        actor_subsystem.destroy_actor(actor)

    _log("Deleted {} previous blockout actors.".format(len(to_delete)))


def _spawn_ground_patch(mesh, material, x, y, size_x_m=100.0, size_y_m=100.0, z=-50.0):
    # Cube is 100 cm. scale=(m, m, thickness_cm/100)
    actor = _spawn_static_mesh(
        mesh=mesh,
        location=unreal.Vector(x, y, z),
        scale=unreal.Vector(size_x_m, size_y_m, 0.5),
        label="KU_GroundPatch",
    )
    _set_material(actor, material)
    return actor


def _spawn_road_segment(mesh, material, x, y, length_m, width_m, z=2.0, label="KU_Road"):
    actor = _spawn_static_mesh(
        mesh=mesh,
        location=unreal.Vector(x, y, z),
        scale=unreal.Vector(length_m, width_m, 0.08),
        label=label,
    )
    _set_material(actor, material)
    return actor


def _grade_z_cm(x_cm):
    return ((x_cm - GRADE_ORIGIN_X_CM) / 100.0) * ROAD_GRADE_M_PER_M * 100.0


def _ensure_player_start_on_road():
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    actors = actor_subsystem.get_all_level_actors()

    player_start = None
    for actor in actors:
        if actor.get_class().get_name() == "PlayerStart":
            player_start = actor
            break

    spawn_x = GRADE_ORIGIN_X_CM
    spawn_y = -220.0
    spawn_z = 140.0 + _grade_z_cm(spawn_x)

    if player_start:
        player_start.set_actor_location(unreal.Vector(spawn_x, spawn_y, spawn_z), False, False)
        player_start.set_actor_rotation(unreal.Rotator(0.0, 0.0, 0.0), False)
    else:
        ps = unreal.EditorLevelLibrary.spawn_actor_from_class(
            unreal.PlayerStart,
            unreal.Vector(spawn_x, spawn_y, spawn_z),
            unreal.Rotator(0.0, 0.0, 0.0),
        )
        ps.tags = list(set(list(ps.tags) + [ROOT_TAG]))
        ps.set_actor_label("KU_PlayerStart")


def _spawn_building_proxy(mesh, x, y, width_m, depth_m, height_m, yaw_deg, label):
    actor = _spawn_static_mesh(
        mesh=mesh,
        location=unreal.Vector(x, y, (height_m * 100.0) * 0.5),
        scale=unreal.Vector(width_m, depth_m, height_m),
        rotation=unreal.Rotator(0.0, yaw_deg, 0.0),
        label=label,
    )
    return actor


def _spawn_roof_cap(mesh, x, y, width_m, depth_m, height_m, label):
    roof = _spawn_static_mesh(
        mesh=mesh,
        location=unreal.Vector(x, y, (height_m * 100.0) + 18.0),
        scale=unreal.Vector(width_m * 0.96, depth_m * 0.96, 0.06),
        label=label,
    )
    return roof


def _spawn_barrier_row(mesh, start_x, start_y, step_m, count, z=45.0):
    barriers = []
    for i in range(count):
        actor = _spawn_static_mesh(
            mesh=mesh,
            location=unreal.Vector(start_x + i * step_m * 100.0, start_y, z),
            scale=unreal.Vector(0.8, 0.4, 0.9),
            label="KU_Barrier",
        )
        barriers.append(actor)
    return barriers


def _spawn_pole(mesh, x_cm, y_cm, height_m=8.0):
    pole = _spawn_static_mesh(
        mesh=mesh,
        location=unreal.Vector(x_cm, y_cm, (height_m * 100.0) * 0.5),
        scale=unreal.Vector(0.14, 0.14, height_m),
        label="KU_Pole",
    )
    arm = _spawn_static_mesh(
        mesh=mesh,
        location=unreal.Vector(x_cm, y_cm, height_m * 100.0 - 40.0),
        scale=unreal.Vector(0.7, 0.05, 0.05),
        label="KU_PoleArm",
    )
    return pole, arm


def _spawn_tree_proxy(mesh, x_cm, y_cm):
    trunk = _spawn_static_mesh(
        mesh=mesh,
        location=unreal.Vector(x_cm, y_cm, 160.0),
        scale=unreal.Vector(0.12, 0.12, 3.2),
        label="KU_TreeTrunk",
    )
    crown = _spawn_static_mesh(
        mesh=mesh,
        location=unreal.Vector(x_cm, y_cm, 430.0),
        scale=unreal.Vector(0.9, 0.9, 0.9),
        label="KU_TreeCrown",
    )
    return trunk, crown


def _spawn_billboard(mesh, x_cm, y_cm, z_cm, w_m=4.0, h_m=2.2, yaw=0.0):
    board = _spawn_static_mesh(
        mesh=mesh,
        location=unreal.Vector(x_cm, y_cm, z_cm),
        scale=unreal.Vector(w_m, 0.08, h_m),
        rotation=unreal.Rotator(0.0, yaw, 0.0),
        label="KU_Billboard",
    )
    return board


def build_scene(clear_existing=True):
    if clear_existing:
        clear_previous_blockout()

    cube_mesh = _asset("/Engine/BasicShapes/Cube.Cube")

    fallback_mat = _asset("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial")
    flat_col_mat = _pick_material([
        "/Game/LevelPrototyping/Materials/M_FlatCol.M_FlatCol",
    ], fallback_mat, "flat_color")

    # Local simple material instances (color-only for now)
    mi_road = _ensure_mi("MI_KU_Road_Asphalt", flat_col_mat, unreal.LinearColor(0.08, 0.08, 0.085, 1.0))
    mi_sidewalk = _ensure_mi("MI_KU_Sidewalk_Concrete", flat_col_mat, unreal.LinearColor(0.62, 0.60, 0.56, 1.0))
    mi_shoulder = _ensure_mi("MI_KU_Shoulder_Dust", flat_col_mat, unreal.LinearColor(0.43, 0.36, 0.30, 1.0))
    mi_facade_concrete = _ensure_mi("MI_KU_Facade_Concrete", flat_col_mat, unreal.LinearColor(0.67, 0.64, 0.60, 1.0))
    mi_facade_blue = _ensure_mi("MI_KU_Facade_Blue", flat_col_mat, unreal.LinearColor(0.10, 0.43, 0.73, 1.0))
    mi_facade_brick = _ensure_mi("MI_KU_Facade_Brick", flat_col_mat, unreal.LinearColor(0.58, 0.30, 0.23, 1.0))
    mi_lane = _ensure_mi("MI_KU_Lane_White", flat_col_mat, unreal.LinearColor(0.90, 0.90, 0.87, 1.0))
    mi_barrier_red = _ensure_mi("MI_KU_Barrier_Red", flat_col_mat, unreal.LinearColor(0.72, 0.10, 0.10, 1.0))
    mi_barrier_white = _ensure_mi("MI_KU_Barrier_White", flat_col_mat, unreal.LinearColor(0.84, 0.84, 0.83, 1.0))
    textured_road_mat = _ensure_textured_road_material("/Game/KalexUrban/Textures/T_KU_Asphalt_D.T_KU_Asphalt_D")

    ground_mat = _pick_material([
        "/Game/LevelPrototyping/Materials/MI_PrototypeGrid_Gray_02.MI_PrototypeGrid_Gray_02",
    ], fallback_mat, "ground")
    road_mat = textured_road_mat if textured_road_mat else (mi_road if mi_road else _pick_material([
        "/Game/StarterContent/Materials/M_Asphalt_Dark.M_Asphalt_Dark",
        "/Game/StarterContent/Materials/M_Asphalt_Gravel.M_Asphalt_Gravel",
        "/Game/LevelPrototyping/Materials/MI_PrototypeGrid_TopDark.MI_PrototypeGrid_TopDark",
        "/Game/LevelPrototyping/Materials/MI_PrototypeGrid_Gray.MI_PrototypeGrid_Gray",
    ], fallback_mat, "road"))
    sidewalk_mat = mi_sidewalk if mi_sidewalk else _pick_material([
        "/Game/StarterContent/Materials/M_Concrete_Poured.M_Concrete_Poured",
        "/Game/LevelPrototyping/Materials/MI_DefaultColorway.MI_DefaultColorway",
        "/Game/LevelPrototyping/Materials/MI_PrototypeGrid_Gray.MI_PrototypeGrid_Gray",
    ], fallback_mat, "sidewalk")
    building_mat = mi_facade_concrete if mi_facade_concrete else _pick_material([
        "/Game/StarterContent/Materials/M_Brick_Clay_New.M_Brick_Clay_New",
        "/Game/StarterContent/Materials/M_Concrete_Tiles.M_Concrete_Tiles",
        "/Game/LevelPrototyping/Materials/MI_PrototypeGrid_Gray.MI_PrototypeGrid_Gray",
    ], fallback_mat, "building")
    marker_mat = mi_lane if mi_lane else _pick_material([
        "/Game/LevelPrototyping/Materials/MI_DefaultColorway.MI_DefaultColorway",
    ], fallback_mat, "marker")
    facade_mats = [m for m in [mi_facade_blue, mi_facade_concrete, mi_facade_brick, building_mat] if m]
    dark_mat = _first_asset([
        "/Game/LevelPrototyping/Materials/MI_PrototypeGrid_TopDark.MI_PrototypeGrid_TopDark",
        "/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial",
    ])
    if not facade_mats:
        facade_mats = [building_mat]

    _log("Building terrain base patches...")

    # Terrain base (smaller footprint for urban SJL block)
    patch_size_m = 100.0
    grid_half = 5  # 11 x 11 patches = 1100m

    for ix in range(-grid_half, grid_half + 1):
        for iy in range(-grid_half, grid_half + 1):
            x = ix * patch_size_m * 100.0
            y = iy * patch_size_m * 100.0

            # Keep avenue center mostly flat, add slight rise toward background (+X)
            rise_cm = max(0.0, ix * 65.0)
            noise_cm = (math.sin(ix * 0.9) + math.cos(iy * 0.7)) * 3.0
            z = -50.0 + rise_cm + noise_cm

            _spawn_ground_patch(cube_mesh, ground_mat, x, y, patch_size_m, patch_size_m, z)

    _log("Building avenue and sidewalks...")

    avenue_length_m = AVENUE_LENGTH_M
    avenue_width_m = CARRIAGEWAY_WIDTH_M * 2.0 + MEDIAN_WIDTH_M
    left_buffer_m = LEFT_SETBACK_M
    right_market_m = RIGHT_SETBACK_M

    # Main avenue body
    road_pitch_deg = math.degrees(math.atan(ROAD_GRADE_M_PER_M))

    avenue_main = _spawn_road_segment(
        mesh=cube_mesh,
        material=road_mat,
        x=(avenue_length_m * 100.0) * 0.5,
        y=0.0,
        length_m=avenue_length_m,
        width_m=avenue_width_m,
        z=5.0 + _grade_z_cm((avenue_length_m * 100.0) * 0.5),
        label="KU_Avenue_Main",
    )
    avenue_main.set_actor_rotation(unreal.Rotator(road_pitch_deg, 0.0, 0.0), False)
    _set_color_if_possible(avenue_main, flat_col_mat, unreal.LinearColor(0.07, 0.07, 0.08, 1.0))

    # Sidewalks
    sidewalk_width_m = SIDEWALK_WIDTH_M
    left_y = -(avenue_width_m * 0.5 + sidewalk_width_m * 0.5)
    right_y = avenue_width_m * 0.5 + sidewalk_width_m * 0.5
    sidewalk_l = _spawn_road_segment(cube_mesh, sidewalk_mat, (avenue_length_m * 100.0) * 0.5, left_y * 100.0, avenue_length_m, sidewalk_width_m, z=14.0 + _grade_z_cm((avenue_length_m * 100.0) * 0.5), label="KU_Sidewalk_L")
    sidewalk_r = _spawn_road_segment(cube_mesh, sidewalk_mat, (avenue_length_m * 100.0) * 0.5, right_y * 100.0, avenue_length_m, sidewalk_width_m, z=14.0 + _grade_z_cm((avenue_length_m * 100.0) * 0.5), label="KU_Sidewalk_R")
    sidewalk_l.set_actor_rotation(unreal.Rotator(road_pitch_deg, 0.0, 0.0), False)
    sidewalk_r.set_actor_rotation(unreal.Rotator(road_pitch_deg, 0.0, 0.0), False)
    _set_color_if_possible(sidewalk_l, flat_col_mat, unreal.LinearColor(0.64, 0.62, 0.57, 1.0))
    _set_color_if_possible(sidewalk_r, flat_col_mat, unreal.LinearColor(0.64, 0.62, 0.57, 1.0))

    # Dirt shoulder strips (typical dusty urban edges)
    shoulder_w_m = 1.4
    left_shoulder_y = -(avenue_width_m * 0.5 + sidewalk_width_m + shoulder_w_m * 0.5)
    right_shoulder_y = avenue_width_m * 0.5 + sidewalk_width_m + shoulder_w_m * 0.5
    shoulder_l = _spawn_road_segment(cube_mesh, mi_shoulder if mi_shoulder else ground_mat, (avenue_length_m * 100.0) * 0.5, left_shoulder_y * 100.0, avenue_length_m, shoulder_w_m, z=7.0 + _grade_z_cm((avenue_length_m * 100.0) * 0.5), label="KU_Shoulder_L")
    shoulder_r = _spawn_road_segment(cube_mesh, mi_shoulder if mi_shoulder else ground_mat, (avenue_length_m * 100.0) * 0.5, right_shoulder_y * 100.0, avenue_length_m, shoulder_w_m, z=7.0 + _grade_z_cm((avenue_length_m * 100.0) * 0.5), label="KU_Shoulder_R")
    shoulder_l.set_actor_rotation(unreal.Rotator(road_pitch_deg, 0.0, 0.0), False)
    shoulder_r.set_actor_rotation(unreal.Rotator(road_pitch_deg, 0.0, 0.0), False)
    _set_color_if_possible(shoulder_l, flat_col_mat, unreal.LinearColor(0.42, 0.36, 0.30, 1.0))
    _set_color_if_possible(shoulder_r, flat_col_mat, unreal.LinearColor(0.42, 0.36, 0.30, 1.0))

    # Central divider strip
    median_actor = _spawn_road_segment(
        cube_mesh,
        sidewalk_mat,
        (avenue_length_m * 100.0) * 0.5,
        0.0,
        avenue_length_m,
        MEDIAN_WIDTH_M,
        z=20.0 + _grade_z_cm((avenue_length_m * 100.0) * 0.5),
        label="KU_Median",
    )
    median_actor.set_actor_rotation(unreal.Rotator(road_pitch_deg, 0.0, 0.0), False)
    _set_color_if_possible(median_actor, flat_col_mat, unreal.LinearColor(0.52, 0.50, 0.46, 1.0))

    # Temporary barriers on right side lane split
    barrier_start_x = 110.0 * 100.0
    barrier_end_x = 390.0 * 100.0
    barrier_step_m = 2.8
    count = int((barrier_end_x - barrier_start_x) / (barrier_step_m * 100.0))
    barrier_y = 4.2 * 100.0
    barrier_actors = _spawn_barrier_row(cube_mesh, barrier_start_x, barrier_y, barrier_step_m, count)
    for i, barrier in enumerate(barrier_actors):
        if i % 2 == 0:
            _set_material(barrier, mi_barrier_red if mi_barrier_red else facade_mats[0])
        else:
            _set_material(barrier, mi_barrier_white if mi_barrier_white else facade_mats[-1])
        if i % 2 == 0:
            _set_color_if_possible(barrier, flat_col_mat, unreal.LinearColor(0.73, 0.10, 0.10, 1.0))
        else:
            _set_color_if_possible(barrier, flat_col_mat, unreal.LinearColor(0.83, 0.83, 0.82, 1.0))
        loc = barrier.get_actor_location()
        barrier.set_actor_location(unreal.Vector(loc.x, loc.y, loc.z + _grade_z_cm(loc.x)), False, False)

    # Dashed lane markers for readability
    dash_len_m = 3.0
    gap_m = 7.0
    lane_line_y_m = -2.1
    x_m = 14.0
    while x_m < avenue_length_m - 10.0:
        marker = _spawn_static_mesh(
            mesh=cube_mesh,
            location=unreal.Vector(x_m * 100.0, lane_line_y_m * 100.0, 9.0 + _grade_z_cm(x_m * 100.0)),
            scale=unreal.Vector(dash_len_m, 0.08, 0.02),
            label="KU_LaneDash",
        )
        _set_material(marker, marker_mat)
        _set_color_if_possible(marker, flat_col_mat, unreal.LinearColor(0.92, 0.92, 0.90, 1.0))
        x_m += dash_len_m + gap_m

    lane_line_y_m_r = 2.1
    x_m = 14.0
    while x_m < avenue_length_m - 10.0:
        marker_r = _spawn_static_mesh(
            mesh=cube_mesh,
            location=unreal.Vector(x_m * 100.0, lane_line_y_m_r * 100.0, 9.0 + _grade_z_cm(x_m * 100.0)),
            scale=unreal.Vector(dash_len_m, 0.08, 0.02),
            label="KU_LaneDash",
        )
        _set_material(marker_r, marker_mat)
        _set_color_if_possible(marker_r, flat_col_mat, unreal.LinearColor(0.92, 0.92, 0.90, 1.0))
        x_m += dash_len_m + gap_m

    # Cross streets from map rhythm (narrower than avenue)
    cross_width_m = 10.0
    cross_points_m = [55.0, 95.0, 135.0, 170.0, 210.0, 250.0, 290.0, 325.0, 365.0]
    cross_reach_m = 34.0
    for cp in cross_points_m:
        cross_actor = _spawn_road_segment(
            cube_mesh,
            road_mat,
            cp * 100.0,
            0.0,
            cross_width_m,
            avenue_width_m + (cross_reach_m * 2.0),
            z=6.0 + _grade_z_cm(cp * 100.0),
            label="KU_CrossStreet",
        )
        _set_color_if_possible(cross_actor, flat_col_mat, unreal.LinearColor(0.09, 0.09, 0.10, 1.0))

    # Parallel local streets to emulate dense SJL grid
    local_street_w_m = 9.0
    local_street_len_m = avenue_length_m * 0.9
    _spawn_road_segment(cube_mesh, road_mat, (avenue_length_m * 100.0) * 0.5, 7000.0, local_street_len_m, local_street_w_m, z=5.0 + _grade_z_cm((avenue_length_m * 100.0) * 0.5), label="KU_LocalStreet_N")
    _spawn_road_segment(cube_mesh, road_mat, (avenue_length_m * 100.0) * 0.5, -7000.0, local_street_len_m, local_street_w_m, z=5.0 + _grade_z_cm((avenue_length_m * 100.0) * 0.5), label="KU_LocalStreet_S")

    # Diagonal avenue crossing (Rio Mayo style)
    diag = _spawn_road_segment(cube_mesh, road_mat, 30500.0, 2900.0, 185.0, 11.0, z=6.0 + _grade_z_cm(30500.0), label="KU_Diagonal_Ave")
    diag.set_actor_rotation(unreal.Rotator(0.0, -38.0, 0.0), False)

    # Access connectors between avenue and local streets
    for cp in [80.0, 160.0, 240.0, 320.0]:
        _spawn_road_segment(cube_mesh, road_mat, cp * 100.0, 3500.0, 8.0, 70.0, z=5.5 + _grade_z_cm(cp * 100.0), label="KU_Connector_N")
        _spawn_road_segment(cube_mesh, road_mat, cp * 100.0, -3500.0, 8.0, 70.0, z=5.5 + _grade_z_cm(cp * 100.0), label="KU_Connector_S")

    _log("Building side building proxies...")

    # Building masses to lock street scale until final assets arrive
    rng = random.Random(20260514)
    step_m = 16.0
    x_m = 24.0
    while x_m < avenue_length_m - 30.0:
        left_h = rng.uniform(2.8, 4.6)
        right_h = rng.uniform(2.8, 5.0)
        left_w = rng.uniform(8.0, 14.0)
        right_w = rng.uniform(7.0, 13.0)
        depth_l = rng.uniform(7.0, 11.0)
        depth_r = rng.uniform(6.0, 10.0)

        left_center_y_m = -((avenue_width_m * 0.5) + sidewalk_width_m + depth_l * 0.5 + left_buffer_m)
        right_center_y_m = (avenue_width_m * 0.5) + sidewalk_width_m + depth_r * 0.5 + right_market_m

        a_l = _spawn_building_proxy(
            cube_mesh,
            x_m * 100.0,
            left_center_y_m * 100.0,
            width_m=left_w,
            depth_m=depth_l,
            height_m=left_h,
            yaw_deg=0.0,
            label="KU_Bldg_L",
        )
        loc = a_l.get_actor_location()
        a_l.set_actor_location(unreal.Vector(loc.x, loc.y, loc.z + _grade_z_cm(loc.x)), False, False)
        facade_l = rng.choice(facade_mats)
        _set_material(a_l, facade_l if facade_l else building_mat)
        _set_color_if_possible(
            a_l,
            flat_col_mat,
            rng.choice([
                unreal.LinearColor(0.12, 0.43, 0.74, 1.0),  # intense blue
                unreal.LinearColor(0.82, 0.77, 0.69, 1.0),  # warm concrete
                unreal.LinearColor(0.64, 0.33, 0.25, 1.0),  # brick
                unreal.LinearColor(0.55, 0.58, 0.61, 1.0),  # gray cement
            ]),
        )
        roof_l = _spawn_roof_cap(cube_mesh, x_m * 100.0, left_center_y_m * 100.0, left_w, depth_l, left_h, "KU_Roof_L")
        _set_material(roof_l, sidewalk_mat)
        _set_color_if_possible(roof_l, flat_col_mat, unreal.LinearColor(0.40, 0.39, 0.37, 1.0))
        loc = roof_l.get_actor_location()
        roof_l.set_actor_location(unreal.Vector(loc.x, loc.y, loc.z + _grade_z_cm(loc.x)), False, False)

        a_r = _spawn_building_proxy(
            cube_mesh,
            x_m * 100.0,
            right_center_y_m * 100.0,
            width_m=right_w,
            depth_m=depth_r,
            height_m=right_h,
            yaw_deg=0.0,
            label="KU_Bldg_R",
        )
        loc = a_r.get_actor_location()
        a_r.set_actor_location(unreal.Vector(loc.x, loc.y, loc.z + _grade_z_cm(loc.x)), False, False)
        facade_r = rng.choice(facade_mats)
        _set_material(a_r, facade_r if facade_r else building_mat)
        _set_color_if_possible(
            a_r,
            flat_col_mat,
            rng.choice([
                unreal.LinearColor(0.10, 0.48, 0.78, 1.0),
                unreal.LinearColor(0.78, 0.73, 0.64, 1.0),
                unreal.LinearColor(0.60, 0.31, 0.22, 1.0),
                unreal.LinearColor(0.58, 0.61, 0.64, 1.0),
            ]),
        )
        roof_r = _spawn_roof_cap(cube_mesh, x_m * 100.0, right_center_y_m * 100.0, right_w, depth_r, right_h, "KU_Roof_R")
        _set_material(roof_r, sidewalk_mat)
        _set_color_if_possible(roof_r, flat_col_mat, unreal.LinearColor(0.38, 0.37, 0.35, 1.0))
        loc = roof_r.get_actor_location()
        roof_r.set_actor_location(unreal.Vector(loc.x, loc.y, loc.z + _grade_z_cm(loc.x)), False, False)

        x_m += step_m

    # Commercial strip canopies (right side)
    x_m = 30.0
    while x_m < avenue_length_m - 25.0:
        awning = _spawn_static_mesh(
            mesh=cube_mesh,
            location=unreal.Vector(x_m * 100.0, (avenue_width_m * 0.5 + sidewalk_width_m + 260.0 / 100.0) * 100.0, 280.0 + _grade_z_cm(x_m * 100.0)),
            scale=unreal.Vector(4.0, 1.2, 0.08),
            label="KU_Awning",
        )
        _set_material(awning, rng.choice(facade_mats))
        x_m += 10.0

    # Commercial signs / brand blocks
    sign_specs = [
        (70.0, 1450.0, 620.0, 8.0, 2.8, 0.0, unreal.LinearColor(0.06, 0.28, 0.78, 1.0)),
        (170.0, 1450.0, 580.0, 4.8, 2.2, 0.0, unreal.LinearColor(0.82, 0.38, 0.14, 1.0)),
        (265.0, 1450.0, 640.0, 5.2, 2.4, 0.0, unreal.LinearColor(0.10, 0.53, 0.76, 1.0)),
    ]
    for sx, sy, sz, sw, sh, syaw, scol in sign_specs:
        sign = _spawn_billboard(cube_mesh, sx * 100.0, sy, sz + _grade_z_cm(sx * 100.0), sw, sh, syaw)
        _set_color_if_possible(sign, flat_col_mat, scol)

    # Utility poles + overhead cables
    pole_left_y_cm = -((avenue_width_m * 0.5 + sidewalk_width_m + 0.6) * 100.0)
    pole_right_y_cm = ((avenue_width_m * 0.5 + sidewalk_width_m + 0.6) * 100.0)
    pole_xs = []
    x_m = 20.0
    while x_m < avenue_length_m - 12.0:
        x_cm = x_m * 100.0
        pole_xs.append(x_cm)
        pl, al = _spawn_pole(cube_mesh, x_cm, pole_left_y_cm, height_m=8.5)
        pr, ar = _spawn_pole(cube_mesh, x_cm, pole_right_y_cm, height_m=8.5)
        for obj in [pl, al, pr, ar]:
            loc = obj.get_actor_location()
            obj.set_actor_location(unreal.Vector(loc.x, loc.y, loc.z + _grade_z_cm(loc.x)), False, False)
        _set_material(pl, dark_mat)
        _set_material(al, dark_mat)
        _set_material(pr, dark_mat)
        _set_material(ar, dark_mat)
        x_m += 22.0

    if len(pole_xs) >= 2:
        span_m = (pole_xs[-1] - pole_xs[0]) / 100.0
        for y_cm in [pole_left_y_cm, pole_right_y_cm]:
            for z_cm in [760.0, 790.0, 820.0]:
                cable = _spawn_static_mesh(
                    mesh=cube_mesh,
                    location=unreal.Vector((pole_xs[0] + pole_xs[-1]) * 0.5, y_cm, z_cm + _grade_z_cm((pole_xs[0] + pole_xs[-1]) * 0.5)),
                    scale=unreal.Vector(span_m * 0.5, 0.01, 0.01),
                    label="KU_Cable",
                )
                cable.set_actor_rotation(unreal.Rotator(math.degrees(math.atan(ROAD_GRADE_M_PER_M)), 0.0, 0.0), False)
                _set_material(cable, dark_mat)

    # Tree line at left edge + small median vegetation
    x_m = 32.0
    while x_m < avenue_length_m - 20.0:
        trunk, crown = _spawn_tree_proxy(cube_mesh, x_m * 100.0, -((avenue_width_m * 0.5) + sidewalk_width_m + 160.0))
        for obj in [trunk, crown]:
            loc = obj.get_actor_location()
            obj.set_actor_location(unreal.Vector(loc.x, loc.y, loc.z + _grade_z_cm(loc.x)), False, False)
        _set_material(trunk, dark_mat)
        _set_material(crown, ground_mat)
        x_m += 24.0

    x_m = 42.0
    while x_m < avenue_length_m - 20.0:
        shrub = _spawn_static_mesh(
            mesh=cube_mesh,
            location=unreal.Vector(x_m * 100.0, 0.0, 55.0 + _grade_z_cm(x_m * 100.0)),
            scale=unreal.Vector(0.35, 0.35, 0.35),
            label="KU_MedianPlant",
        )
        _set_material(shrub, ground_mat)
        x_m += 18.0

    _ensure_player_start_on_road()

    _log("Blockout complete. Save level if result looks good.")


if __name__ == "__main__":
    try:
        build_scene(clear_existing=True)
    except Exception as exc:
        _warn("Build failed: {}".format(exc))
