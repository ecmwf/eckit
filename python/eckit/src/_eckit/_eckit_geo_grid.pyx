# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0


cimport eckit_geo_area
cimport eckit_geo_grid
from cpython.pycapsule cimport PyCapsule_New
from cython.operator cimport dereference
from libcpp.utility cimport pair
from libcpp.vector cimport vector

from eckit.geo._eckit_geo import _spec_str
from eckit.geo._eckit_geo_figure import Figure
from eckit.geo._eckit_geo_projection import Projection


cdef class Grid:
    cdef const eckit_geo_grid.Grid* _grid

    def __dealloc__(self):
        if self._grid != NULL:
            del self._grid

    def __cinit__(self, spec = None, **kwargs):
        self._grid = NULL
        if spec is None and not kwargs:
            return  # internal use only (to_unstructured_ll)
        assert bool(spec) != bool(kwargs)

        if kwargs or isinstance(spec, dict):
            spec = kwargs if kwargs else spec

            # coordinates are passed directly avoiding a spec string
            has_coordinates = "latitudes" in spec and "longitudes" in spec
            if has_coordinates and set(spec) <= {"latitudes", "longitudes", "name"}:
                self._grid = new eckit_geo_grid.Unstructured(
                    spec["longitudes"], spec["latitudes"], spec.get("name", "")
                )
                return

            spec = _spec_str(spec)

        assert isinstance(spec, str)
        self._grid = eckit_geo_grid.GridFactory.make_from_string(spec)

    def __eq__(self, other) -> bool:
        if not isinstance(other, Grid):
            return NotImplemented
        return self.spec_str == other.spec_str

    def to_latlons(self):
        cdef pair[vector[double], vector[double]] latlons = self._grid.to_latlons()
        return list(latlons.first), list(latlons.second)

    def to_xyz(self):
        cdef vector[vector[double]] xyz = self._grid.to_xyz()
        return list(xyz[0]), list(xyz[1]), list(xyz[2])

    def to_unstructured_ll(self, name: str = ""):
        cdef Grid grid = Grid.__new__(Grid)  # wrap pointer directly
        grid._grid = self._grid.to_unstructured_ll(name)
        return grid

    def distinct_latitudes(self):
        return self.lat()

    def distinct_longitudes(self):
        return self.lon()

    def x(self):
        return list(eckit_geo_grid.grid_x_values(dereference(self._grid)))

    def y(self):
        return list(eckit_geo_grid.grid_y_values(dereference(self._grid)))

    def lon(self):
        return list(eckit_geo_grid.grid_lon_values(dereference(self._grid)))

    def lat(self):
        return list(eckit_geo_grid.grid_lat_values(dereference(self._grid)))

    def bounding_box(self) -> tuple:
        cdef const eckit_geo_area.BoundingBox* bbox = &self._grid.boundingBox()
        return bbox.north(), bbox.west(), bbox.south(), bbox.east()

    def grid_box_areas(self):
        try:
            import mir
        except ModuleNotFoundError:
            return None

        return mir.grid_box_areas(self)

    def search(self, **spec):
        """Search on the grid points (see eckit.geo.Search)."""
        from eckit.geo._eckit_geo_search import Search

        return Search(self, **spec)

    def _capsule(self):
        """The C++ grid, for other extension modules."""
        return PyCapsule_New(<void*>self._grid, "eckit::geo::Grid", NULL)

    @property
    def figure(self) -> Figure:
        return Figure._borrow(
            PyCapsule_New(<void*>&self._grid.figure(), "eckit::geo::Figure", NULL),
            self,
        )

    @property
    def projection(self) -> Projection:
        return Projection._borrow(
            PyCapsule_New(
                <void*>&self._grid.projection(), "eckit::geo::Projection", NULL
            ),
            self,
        )

    @property
    def spec_str(self) -> str:
        return self._grid.spec_str()

    @property
    def spec(self) -> dict:
        from json import loads
        return loads(self.spec_str)

    @property
    def catalog_str(self) -> str:
        return self._grid.catalog_str()

    @property
    def catalog(self) -> dict:
        from json import loads
        return loads(self.catalog_str)

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
        return tuple(self._grid.shape())

    def size(self) -> int:
        return self._grid.size()

    def __len__(self) -> int:
        return self.size()
