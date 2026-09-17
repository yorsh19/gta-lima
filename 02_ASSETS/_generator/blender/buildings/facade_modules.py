"""Solid frame pieces around real openings; no wall spans a door/window opening."""
import bpy
from common.mesh_utils import create_box, create_cylinder, join_objects, set_origin_ground_center
from common.export_utils import export_fbx
from config import BUILDING_MODULAR

HEIGHT = 2.80
DEPTH = 0.20


def export_parts(name, parts):
    # One convex collision per solid part, so a hull cannot bridge the opening.
    collisions = []
    for index, part in enumerate(parts):
        collision = part.copy()
        collision.data = part.data.copy()
        collision.name = f"UCX_{name}_{index:02d}"
        bpy.context.collection.objects.link(collision)
        collisions.append(collision)
    obj = join_objects(parts, name)
    set_origin_ground_center(obj)
    export_fbx(obj, BUILDING_MODULAR, collision_objects=collisions)
    for item in [obj] + collisions:
        bpy.data.objects.remove(item, do_unlink=True)


def opening_module(kind, centimeters, materials):
    width = centimeters / 100
    jamb = 0.08
    bottom = 1.0 if kind == "Window" else 0.0
    top = 2.20 if kind != "Garage" else 2.35
    opening_width = width - 2 * jamb
    parts = []
    def box(label, size, location, material="brick"):
        parts.append(create_box(label, size, location, materials[material]))
    for side in (-1, 1):
        box("Jamb", (jamb, DEPTH, HEIGHT), (side*(width-jamb)/2, 0, HEIGHT/2))
    box("Lintel", (opening_width, DEPTH, HEIGHT-top), (0, 0, (HEIGHT+top)/2))
    if bottom:
        box("Sill", (opening_width, DEPTH, bottom), (0, 0, bottom/2))
    # The closure is a separate material slot inside the opening, never masonry.
    box("Glass" if kind == "Window" else "Gate", (opening_width, .035, top-bottom),
        (0, .025, (bottom+top)/2), "glass" if kind == "Window" else "metal")
    if kind == "Window":
        for side in (-1, 1):
            box("Frame", (.035, .06, top-bottom),
                (side*(opening_width-.035)/2, -.025, (bottom+top)/2), "metal")
        for z in (bottom+.02, top-.02):
            box("Frame", (opening_width, .06, .04), (0, -.025, z), "metal")
        for i in range(1, max(2, int(opening_width/.20))):
            x = -opening_width/2 + i*opening_width/max(2, int(opening_width/.20))
            box("Grille", (.018, .04, top-bottom), (x, -.065, (bottom+top)/2), "metal")
    export_parts(f"SM_SJL_Facade_{kind}_{centimeters:03d}", parts)


def generate(materials):
    for width in (50, 100, 150, 200):
        export_parts(f"SM_SJL_Facade_Wall_{width:03d}", [
            create_box("Wall", (width/100, DEPTH, HEIGHT), (0,0,HEIGHT/2), materials["brick"])])
    for kind, widths in (("Window", (100,120,150)), ("Door",(90,100)), ("Garage",(250,300))):
        for width in widths:
            opening_module(kind, width, materials)
    for name, size in (("Column_025",(.25,.25,2.8)), ("Beam_025",(1,.25,.25))):
        export_parts("SM_SJL_"+name, [
            create_box(name, size, (0,0,size[2]/2), materials["concrete"])])
    for name, positions in (("Rebar_01",[(0,0)]),
                            ("Rebar_Group_01",[(-.06,-.06),(.06,-.06),(-.06,.06),(.06,.06)])):
        export_parts("SM_SJL_"+name, [
            create_cylinder("Rod", .009, .9, (x,y,.45), 8, materials["rust"])
            for x,y in positions])
