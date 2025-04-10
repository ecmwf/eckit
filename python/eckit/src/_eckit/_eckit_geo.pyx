# (C) Copyright 2025- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.


cimport eckit
cimport eckit_geo
from libcpp.utility cimport pair
from libcpp.vector cimport vector

eckit.eckit_main_initialise()


# cdef class Area:
#     cdef const eckit_geo.Area* _area
#
#     def __cinit__(self, spec = None, **kwargs):
#         assert bool(spec) != bool(kwargs)
#
#         if kwargs or isinstance(spec, dict):
#             from yaml import dump
#             spec = dump(kwargs if kwargs else spec, default_flow_style=True).strip()
#
#         try:
#             assert isinstance(spec, str)
#             self._area = eckit_geo.AreaFactory.make_from_string(spec.encode())
#
#         except RuntimeError as e:
#             # opportunity to do something interesting
#             raise
#
#     def __eq__(self, other) -> bool:
#         if not isinstance(other, Grid):
#             return NotImplemented
#         return self.spec_str == other.spec_str
#
#     @property
#     def spec_str(self) -> str:
#         return self._area.spec_str().decode()
#
#     @property
#     def spec(self) -> dict:
#         from yaml import safe_load
#         return safe_load(self.spec_str)
#
#     @property
#     def type(self) -> str:
#         return self._area.type().decode()


cdef class Grid:
    cdef const eckit_geo.Grid* _grid

    def __cinit__(self, spec = None, **kwargs):
        assert bool(spec) != bool(kwargs)

        if kwargs or isinstance(spec, dict):
            from yaml import dump
            spec = dump(kwargs if kwargs else spec, default_flow_style=True).strip()

        try:
            assert isinstance(spec, str)
            self._grid = eckit_geo.GridFactory.make_from_string(spec.encode())

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

    def bounding_box(self) -> tuple:
        cdef const eckit_geo.BoundingBox* bbox = &self._grid.boundingBox()
        cdef double north = bbox.north
        cdef double west = bbox.west
        cdef double south = bbox.south
        cdef double east = bbox.east
        return north, west, south, east

    @property
    def spec_str(self) -> str:
        return self._grid.spec_str().decode()

    @property
    def spec(self) -> dict:
        from yaml import safe_load
        return safe_load(self.spec_str)

    @property
    def type(self) -> str:
        return self._grid.type().decode()

    @property
    def shape(self) -> tuple:
        cdef vector[size_t] shape_vec = self._grid.shape()
        return tuple(shape_vec)

    def size(self) -> int:
        return self._grid.size()

    def __len__(self) -> int:
        return self.size()

    def __dealloc__(self):
        del self._grid
