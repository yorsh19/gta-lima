import math


EARTH_RADIUS = 6378137.0


class LocalCoordinateConverter:

    def __init__(self, origin_lat, origin_lon):

        self.origin_lat = origin_lat
        self.origin_lon = origin_lon

        self.origin_lat_rad = math.radians(origin_lat)
        self.origin_lon_rad = math.radians(origin_lon)

    def to_local(self, lat, lon):
        """
        Convierte coordenadas WGS84 (lat/lon) a coordenadas locales
        expresadas en metros para KALEX Urban.

        Sistema utilizado:

            +X = Este
            -X = Oeste

            +Y = Sur
            -Y = Norte

        La inversión del eje norte/sur se realiza aquí para mantener
        exactamente la misma transformación para:

        - carreteras
        - edificios
        - parques
        - árboles
        - amenities
        - cualquier geometría OSM futura

        No volver a invertir Y en process_osm.py ni en Unreal.
        """

        lat_rad = math.radians(lat)
        lon_rad = math.radians(lon)

        delta_lon = lon_rad - self.origin_lon_rad
        delta_lat = lat_rad - self.origin_lat_rad

        # Distancia Este/Oeste.
        x = (
            EARTH_RADIUS
            * delta_lon
            * math.cos(self.origin_lat_rad)
        )

        # Distancia Norte/Sur.
        north = EARTH_RADIUS * delta_lat

        # Conversión al sistema utilizado por KALEX Urban / UE.
        y = -north

        return x, y