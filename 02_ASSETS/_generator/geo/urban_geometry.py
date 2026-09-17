"""Urban block/lot derivation for KALEX Urban. Coordinates are local metres."""
import math, random
try:
    from shapely.geometry import LineString, Polygon, Point
    from shapely.ops import unary_union, polygonize
except ImportError as e:
    raise SystemExit("Missing dependency 'shapely'. Run: py -3.12 -m pip install shapely") from e


def _road_width(r): return max(2.0, float(r.get('width') or 6.0))

def derive_blocks(roads, parks, clip_radius=520.0, min_area=120.0, max_area=90000.0):
    lines=[]
    for r in roads:
        pts=[(p[0],p[1]) for p in r.get('points',[]) if len(p)>=2]
        if len(pts)>=2: lines.append(LineString(pts))
    if not lines: return []
    network=unary_union(lines)
    polys=list(polygonize(network))
    park_polys=[]
    for p in parks:
        q=p.get('polygon',[])
        if len(q)>=3:
            g=Polygon(q)
            if g.is_valid: park_polys.append(g)
    out=[]
    for i,p in enumerate(polys):
        if not p.is_valid: p=p.buffer(0)
        if p.is_empty or p.area<min_area or p.area>max_area: continue
        c=p.centroid
        if abs(c.x)>clip_radius or abs(c.y)>clip_radius: continue
        # Pull block edge away from road centre lines; conservative 2.5 m.
        inner=p.buffer(-2.5, join_style=2)
        if inner.is_empty or inner.area<min_area: continue
        if inner.geom_type=='MultiPolygon': inner=max(inner.geoms,key=lambda g:g.area)
        # Exclude polygons mostly occupied by mapped parks.
        if any(inner.intersection(pg).area/max(inner.area,1)>0.45 for pg in park_polys): continue
        coords=list(inner.exterior.coords)[:-1]
        out.append({'id':i+1,'area':round(inner.area,2),'polygon':[[round(x,3),round(y,3)] for x,y in coords]})
    return out


def _oriented_frame(poly):
    rect=poly.minimum_rotated_rectangle
    pts=list(rect.exterior.coords)[:-1]
    edges=[]
    for i in range(4):
        a,b=pts[i],pts[(i+1)%4]
        dx,dy=b[0]-a[0],b[1]-a[1]
        edges.append((math.hypot(dx,dy),dx,dy))
    _,dx,dy=max(edges,key=lambda e:e[0])
    angle=math.atan2(dy,dx)
    ca,sa=math.cos(-angle),math.sin(-angle)
    def to_local(x,y): return (x*ca-y*sa,x*sa+y*ca)
    def to_world(x,y):
        ca2,sa2=math.cos(angle),math.sin(angle)
        return (x*ca2-y*sa2,x*sa2+y*ca2)
    loc=[to_local(x,y) for x,y in poly.exterior.coords]
    xs=[p[0] for p in loc]; ys=[p[1] for p in loc]
    return angle,to_local,to_world,min(xs),max(xs),min(ys),max(ys)


def derive_lots(blocks, seed=15412, frontage=8.0, depth=18.0):
    """Create conservative rectangular lots clipped to blocks. Skips invalid cells."""
    rng=random.Random(seed); lots=[]; lid=1
    for b in blocks:
        poly=Polygon(b['polygon'])
        if not poly.is_valid: poly=poly.buffer(0)
        if poly.is_empty: continue
        angle,to_local,to_world,minx,maxx,miny,maxy=_oriented_frame(poly)
        # choose rows from both long sides toward centre, avoiding deep interior duplication
        spanx=maxx-minx; spany=maxy-miny
        fw=max(6.0,min(10.0,frontage+rng.uniform(-1.5,1.5)))
        dp=max(10.0,min(depth,spany*0.48))
        nx=max(1,int(spanx/fw)); actual=spanx/nx
        for side in (0,1):
            y0=miny if side==0 else maxy-dp
            for ix in range(nx):
                x0=minx+ix*actual
                margin=0.35
                corners_local=[(x0+margin,y0+margin),(x0+actual-margin,y0+margin),(x0+actual-margin,y0+dp-margin),(x0+margin,y0+dp-margin)]
                corners=[to_world(x,y) for x,y in corners_local]
                lotpoly=Polygon(corners)
                # Keep only cells substantially inside the true irregular block.
                if lotpoly.area<45 or poly.intersection(lotpoly).area/lotpoly.area<0.92: continue
                c=lotpoly.centroid
                floors=rng.choices([1,2,3,4,5],[18,38,28,12,4])[0]
                lots.append({
                    'id':lid,'blockId':b['id'],'center':[round(c.x,3),round(c.y,3)],
                    'width':round(actual-2*margin,3),'depth':round(dp-2*margin,3),
                    'yaw':round(math.degrees(angle),3),'floors':floors,
                    'commercial':rng.random()<0.16,'unfinished':rng.random()<0.48,
                    'footprint':[[round(x,3),round(y,3)] for x,y in corners]
                }); lid+=1
    return lots


def add_oriented_building_fields(building):
    fp=building.get('footprint',[])
    if len(fp)<3: return building
    poly=Polygon(fp)
    if not poly.is_valid: poly=poly.buffer(0)
    if poly.is_empty: return building
    angle,_,_,minx,maxx,miny,maxy=_oriented_frame(poly)
    c=poly.centroid
    building['center']=[round(c.x,3),round(c.y,3)]
    building['width']=round(maxx-minx,3); building['depth']=round(maxy-miny,3)
    building['yaw']=round(math.degrees(angle),3)
    return building
