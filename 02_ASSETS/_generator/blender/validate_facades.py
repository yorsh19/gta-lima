"""Check exported FBX masonry holes, metric UVs, materials and ground origins."""
import sys
from pathlib import Path
import json
import bpy
from mathutils import Vector
from mathutils.bvhtree import BVHTree

sys.path.insert(0,str(Path(__file__).resolve().parent))
from config import BUILDING_MODULAR
from common.mesh_utils import clear_scene

report = []
for file in sorted(BUILDING_MODULAR.glob("SM_SJL_Facade_*.fbx")):
    clear_scene()
    bpy.ops.import_scene.fbx(filepath=str(file))
    obj = next(o for o in bpy.context.scene.objects if o.type == 'MESH' and not o.name.startswith("UCX_"))
    assert obj.data.uv_layers, file.name
    points = [obj.matrix_world @ v.co for v in obj.data.vertices]
    assert abs(min(v.z for v in points)) < 1e-4, file.name
    width = int(file.stem.rsplit("_",1)[1])/100
    assert abs(max(v.x for v in points)-min(v.x for v in points)-width)<1e-4, file.name
    polygons = [list(p.vertices) for p in obj.data.polygons
                if obj.data.materials[p.material_index].name.startswith("M_SJL_Brick_Red")]
    assert polygons, file.name
    bvh = BVHTree.FromPolygons(points,polygons)
    if "_Wall_" not in file.name:
        # Ray through the centre of the closure must never hit masonry.
        z = 1.6 if "_Window_" in file.name else 1.0
        assert bvh.ray_cast(Vector((0,-1,z)),Vector((0,1,0)),2)[0] is None, file.name
        # The header above that same opening must contain masonry.
        assert bvh.ray_cast(Vector((0,-1,2.6)),Vector((0,1,0)),2)[0] is not None, file.name
    # Per-face UV edge lengths retain metre density after FBX roundtrip.
    uv = obj.data.uv_layers.active.data
    for p in obj.data.polygons:
        if abs(p.normal.y) < .999:
            continue
        loops = list(p.loop_indices)
        a,b = loops[:2]
        pa,pb = (points[obj.data.loops[i].vertex_index] for i in (a,b))
        assert abs((uv[a].uv-uv[b].uv).length-(pa-pb).length)<1e-4, file.name
    report.append(file.stem)
(Path(__file__).resolve().parents[1]/"validation"/"blender_validation.json").write_text(json.dumps(report,indent=2))
print(f"[KALEX] VERIFIED {len(report)} FBX modules: masonry holes, UV scale, ground origins")
