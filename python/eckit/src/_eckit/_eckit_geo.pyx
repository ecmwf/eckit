# (C) Copyright 1996- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.


cimport eckit_geo
from cython.operator cimport dereference
from libcpp.memory cimport unique_ptr
from libcpp.utility cimport pair
from libcpp.vector cimport vector

cimport eckit

eckit.eckit_main_initialise()


def version() -> str:
    return eckit_geo.LibEcKitGeo.instance().version()


def git_sha1() -> str:
    return eckit_geo.LibEcKitGeo.instance().gitsha1(40)


def cache_dir_purge() -> None:
    eckit_geo.LibEcKitGeo.purgeCacheDir()


cdef class Area:
    cdef const eckit_geo.Area* _area

    def __dealloc__(self):
        if self._area != NULL:
            del self._area

    def __cinit__(self, spec = None, **kwargs):
        self._area = NULL
        assert bool(spec) != bool(kwargs)

        if kwargs or isinstance(spec, dict):
            from yaml import dump
            spec = dump(kwargs if kwargs else spec, default_flow_style=True).strip()

        try:
            assert isinstance(spec, str)
            self._area = eckit_geo.AreaFactory.make_from_string(spec)

        except RuntimeError as e:
            # opportunity to do something interesting
            raise

    def __eq__(self, other) -> bool:
        if not isinstance(other, Grid):
            return NotImplemented
        return self.spec_str == other.spec_str

    @property
    def spec_str(self) -> str:
        return self._area.spec_str()

    @property
    def spec(self) -> dict:
        from yaml import safe_load
        return safe_load(self.spec_str)

    @property
    def type(self) -> str:
        return self._area.type()


cdef class BoundingBox:
    cdef eckit_geo.BoundingBox* _bbox

    def __dealloc__(self):
        if self._bbox != NULL:
            del self._bbox

    def __cinit__(self, north=None, west=None, south=None, east=None):
        self._bbox = NULL

        cdef unique_ptr[eckit_geo.BoundingBox] bbox
        bbox = eckit_geo.BoundingBox.make_from_area(
            float(north), float(west), float(south), float(east),
        )
        self._bbox = bbox.release()

    def intersects(self, other) -> bool:
        cdef BoundingBox other_bbox
        cdef bint intersects
        if not isinstance(other, BoundingBox):
            raise TypeError("other must be a BoundingBox")
        other_bbox = <BoundingBox>other
        intersects = eckit_geo.bbox_intersects(
            dereference(self._bbox), dereference(other_bbox._bbox)
        )
        return intersects

    def contains(self, other) -> bool:
        cdef BoundingBox other_bbox
        if not isinstance(other, BoundingBox):
            raise TypeError("other must be a BoundingBox")
        other_bbox = <BoundingBox>other
        return self._bbox.contains(dereference(other_bbox._bbox))

    def contains_point(self, lon, lat) -> bool:
        return eckit_geo.bbox_contains_lonlat(
            dereference(self._bbox), float(lon), float(lat)
        )

    def as_list(self) -> list:
        return [self.north, self.west, self.south, self.east]

    @property
    def spec_str(self) -> str:
        return self._bbox.spec_str()

    @property
    def spec(self) -> dict:
        from yaml import safe_load
        return safe_load(self.spec_str)

    @property
    def north(self) -> float:
        return self._bbox.north()

    @property
    def west(self) -> float:
        return self._bbox.west()

    @property
    def south(self) -> float:
        return self._bbox.south()

    @property
    def east(self) -> float:
        return self._bbox.east()

    @property
    def global_(self) -> bool:
        return self._bbox.is_global()

    @property
    def periodic(self) -> bool:
        return self._bbox.periodic()

    @property
    def empty(self) -> bool:
        return self._bbox.empty()

    @property
    def area(self) -> float:
        return self._bbox.area()

    def __repr__(self) -> str:
        return str(self.as_list())

    __str__ = __repr__


cdef class Figure:
    cdef const eckit_geo.Figure* _figure

    def __dealloc__(self):
        if self._figure != NULL:
            del self._figure

    def __cinit__(self, spec = None, **kwargs):
        self._figure = NULL
        assert bool(spec) != bool(kwargs)

        if kwargs or isinstance(spec, dict):
            from yaml import dump
            spec = dump(kwargs if kwargs else spec, default_flow_style=True).strip()

        try:
            assert isinstance(spec, str)
            self._figure = eckit_geo.FigureFactory.make_from_string(spec)

        except RuntimeError as e:
            # opportunity to do something interesting
            raise

    def __eq__(self, other) -> bool:
        if not isinstance(other, Figure):
            return NotImplemented
        return self.spec_str == other.spec_str

    def area(self, bbox = None) -> float:
        cdef BoundingBox _bbox
        if bbox is None:
            return self._figure.area()
        if not isinstance(bbox, BoundingBox):
            raise TypeError("bbox must be a BoundingBox")
        _bbox = <BoundingBox>bbox
        return self._figure.area(dereference(_bbox._bbox))

    @property
    def R(self) -> float:
        return eckit_geo.figure_R(dereference(self._figure))

    @property
    def a(self) -> float:
        return self._figure.a()

    @property
    def b(self) -> float:
        return self._figure.b()

    @property
    def spec_str(self) -> str:
        return self._figure.spec_str()

    @property
    def spec(self) -> dict:
        from yaml import safe_load
        return safe_load(self.spec_str)

    @property
    def proj_str(self) -> str:
        return self._figure.proj_str()

    @property
    def spherical(self) -> bool:
        return self._figure.spherical()

    @property
    def eccentricity(self) -> float:
        return self._figure.eccentricity()

    @property
    def flattening(self) -> float:
        return self._figure.flattening()


cdef class Grid:
    cdef const eckit_geo.Grid* _grid

    def __dealloc__(self):
        if self._grid != NULL:
            del self._grid

    def __cinit__(self, spec = None, **kwargs):
        self._grid = NULL
        assert bool(spec) != bool(kwargs)

        if kwargs or isinstance(spec, dict):
            from yaml import dump
            spec = dump(kwargs if kwargs else spec, default_flow_style=True).strip()

        try:
            assert isinstance(spec, str)
            self._grid = eckit_geo.GridFactory.make_from_string(spec)

        except RuntimeError as e:
            # opportunity to do something interesting
            raise

    def __eq__(self, other) -> bool:
        if not isinstance(other, Grid):
            return NotImplemented
        return self.spec_str == other.spec_str

    def to_latlons(self):
        cdef pair[vector[double], vector[double]] latlons = self._grid.to_latlons()
        return list(latlons.first), list(latlons.second)

    def distinct_latitudes(self):
        return self.lat()

    def distinct_longitudes(self):
        return self.lon()

    def x(self):
        cdef vector[double] v = eckit_geo.grid_x_values(dereference(self._grid))
        return list(v)

    def y(self):
        cdef vector[double] v = eckit_geo.grid_y_values(dereference(self._grid))
        return list(v)

    def lon(self):
        cdef vector[double] v = eckit_geo.grid_lon_values(dereference(self._grid))
        return list(v)

    def lat(self):
        cdef vector[double] v = eckit_geo.grid_lat_values(dereference(self._grid))
        return list(v)

    def bounding_box(self) -> tuple:
        cdef const eckit_geo.BoundingBox* bbox = &self._grid.boundingBox()
        cdef double north = bbox.north()
        cdef double west = bbox.west()
        cdef double south = bbox.south()
        cdef double east = bbox.east()
        return north, west, south, east

    def grid_box_areas(self):
        try:
            import mir
        except ModuleNotFoundError:
            return None

        return mir.grid_box_areas(self)

    @property
    def spec_str(self) -> str:
        return self._grid.spec_str()

    @property
    def spec(self) -> dict:
        from yaml import safe_load
        return safe_load(self.spec_str)

    @property
    def catalog_str(self) -> str:
        return self._grid.catalog_str()

    @property
    def catalog(self) -> dict:
        from yaml import safe_load
        return safe_load(self.catalog_str)

    @property
    def type(self) -> str:
        return self._grid.type()

    @property
    def uid(self) -> str:
        return self._grid.uid()

    @property
    def order(self) -> str:
        return self._grid.order()

    @property
    def shape(self) -> tuple:
        cdef vector[size_t] shape_vec = self._grid.shape()
        return tuple(shape_vec)

    def size(self) -> int:
        return self._grid.size()

    def __len__(self) -> int:
        return self.size()
