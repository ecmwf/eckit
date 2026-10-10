# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0


cimport eckit_geo_search
from cpython.pycapsule cimport PyCapsule_GetPointer
from cython.operator cimport dereference
from libcpp.vector cimport vector


# Python-friendly spec keys
_KEYS = {
    "tree": "search-tree",
    "k": "search-k",
    "radius": "search-radius",
    "fast_build": "search-fast-build",
}


cdef const eckit_geo_search.Grid* _grid_ptr(grid) except NULL:
    # eckit.geo.Grid is defined in another extension module
    return <const eckit_geo_search.Grid*>PyCapsule_GetPointer(
        grid._capsule(), "eckit::geo::Grid"
    )


cdef vector[double] _point(point) except *:
    return [float(c) for c in point]


cdef class Search:
    """Search on the points of a grid, following k-d tree algorithms.

    Points are (lon, lat) or geocentric (x, y, z); results are (index, distance), by
    increasing distance (chord length, in metres on the Earth).

    Searches are shared by grid and spec (tree, k, radius, search, ..., see
    eckit::geo::Search) in a cache, within its capacity (see search_cache_statistics).
    """
    cdef eckit_geo_search.SearchHandle* _handle

    def __cinit__(self, grid, **spec):
        cdef const eckit_geo_search.Grid* ptr = _grid_ptr(grid)

        from eckit.geo._eckit_geo import _spec_str

        self._handle = NULL
        spec = {_KEYS.get(k, k.replace("_", "-")): v for k, v in spec.items()}
        self._handle = eckit_geo_search.search_get(dereference(ptr), _spec_str(spec))

    def __dealloc__(self):
        if self._handle != NULL:
            del self._handle

    def search(self, point) -> list:
        """Search with the configured behaviour (nearest neighbour by default)."""
        return eckit_geo_search.search_point(dereference(self._handle), _point(point))

    def search_nn(self, point) -> tuple:
        return eckit_geo_search.search_nn(dereference(self._handle), _point(point))

    def search_knn(self, point, k) -> list:
        return eckit_geo_search.search_knn(dereference(self._handle), _point(point), k)

    def search_radius(self, point, radius) -> list:
        return eckit_geo_search.search_radius(
            dereference(self._handle), _point(point), radius
        )

    def search_knn_or_radius(self, point, k, radius) -> list:
        """Union of the k nearest neighbours and the neighbours within radius."""
        return eckit_geo_search.search_knn_or_radius(
            dereference(self._handle), _point(point), k, radius
        )

    def search_knn_and_radius(self, point, k, radius) -> list:
        """Intersection of the k nearest neighbours and the neighbours within radius."""
        return eckit_geo_search.search_knn_and_radius(
            dereference(self._handle), _point(point), k, radius
        )

    @property
    def footprint(self) -> dict:
        """Memory used (bytes), in the process and shared (shared memory, page
        cache)."""
        usage = eckit_geo_search.search_footprint(dereference(self._handle))
        return dict(zip(("memory", "shared"), usage))

    def __len__(self) -> int:
        return eckit_geo_search.search_size(dereference(self._handle))

    def __repr__(self) -> str:
        return eckit_geo_search.search_str(dereference(self._handle))


def search_cache_statistics() -> dict:
    """Cache of searches: hits, misses, evictions, size, and usage (bytes, in the
    process and shared)."""
    keys = ("hits", "misses", "evictions", "size", "memory", "shared")
    return dict(zip(keys, eckit_geo_search.search_cache_statistics()))


def search_cache_clear() -> None:
    """Evict all searches (those in use remain valid)."""
    eckit_geo_search.search_cache_clear()
