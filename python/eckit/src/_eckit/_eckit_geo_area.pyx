# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0


cimport eckit_geo_area
from cpython.pycapsule cimport PyCapsule_New
from cython.operator cimport dereference
from libcpp.memory cimport unique_ptr

from eckit.geo._eckit_geo import _spec_str


cdef class Area:
    cdef const eckit_geo_area.Area* _area

    def __dealloc__(self):
        if self._area != NULL:
            del self._area

    def __cinit__(self, spec = None, **kwargs):
        self._area = NULL
        assert bool(spec) != bool(kwargs)

        if kwargs or isinstance(spec, dict):
            spec = _spec_str(kwargs if kwargs else spec)

        assert isinstance(spec, str)
        self._area = eckit_geo_area.AreaFactory.make_from_string(spec)

    def __eq__(self, other) -> bool:
        if not isinstance(other, Area):
            return NotImplemented
        return self.spec_str == other.spec_str

    @property
    def spec_str(self) -> str:
        return self._area.spec_str()

    @property
    def spec(self) -> dict:
        from json import loads
        return loads(self.spec_str)

    @property
    def type(self) -> str:
        return self._area.type()


cdef class BoundingBox:
    cdef eckit_geo_area.BoundingBox* _bbox

    def __dealloc__(self):
        if self._bbox != NULL:
            del self._bbox

    def __cinit__(self, north=None, west=None, south=None, east=None):
        self._bbox = NULL

        cdef unique_ptr[eckit_geo_area.BoundingBox] bbox
        bbox = eckit_geo_area.BoundingBox.make_from_area(
            float(north), float(west), float(south), float(east),
        )
        self._bbox = bbox.release()

    def intersects(self, other) -> bool:
        cdef BoundingBox other_bbox
        if not isinstance(other, BoundingBox):
            raise TypeError("other must be a BoundingBox")
        other_bbox = <BoundingBox>other
        return eckit_geo_area.bbox_intersects(
            dereference(self._bbox), dereference(other_bbox._bbox)
        )

    def contains(self, other) -> bool:
        cdef BoundingBox other_bbox
        if not isinstance(other, BoundingBox):
            raise TypeError("other must be a BoundingBox")
        other_bbox = <BoundingBox>other
        return self._bbox.contains(dereference(other_bbox._bbox))

    def contains_point(self, lon, lat) -> bool:
        return eckit_geo_area.bbox_contains_lonlat(
            dereference(self._bbox), float(lon), float(lat)
        )

    def as_list(self) -> list:
        return [self.north, self.west, self.south, self.east]

    def _capsule(self):
        """The C++ bounding box, for other extension modules."""
        return PyCapsule_New(<void*>self._bbox, "eckit::geo::area::BoundingBox", NULL)

    @property
    def spec_str(self) -> str:
        return self._bbox.spec_str()

    @property
    def spec(self) -> dict:
        from json import loads
        return loads(self.spec_str)

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
