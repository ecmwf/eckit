# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0


cimport eckit_geo_search
from cpython.pycapsule cimport PyCapsule_GetPointer
from cython.operator cimport dereference
from libc.string cimport memcpy
from libcpp.string cimport string
from libcpp.vector cimport vector


# Python-friendly spec keys
_KEYS = {
    "k": "search-k",
    "radius": "search-radius",
    "tolerance": "search-tolerance",
    "fast_build": "search-fast-build",
}


def _spec(spec: dict) -> str:
    from eckit.geo._eckit_geo import _spec_str

    if "loader" in spec:
        raise ValueError(
            "Search: the k-d tree loader is of the library (ECKIT_GEO_SEARCH_LOADER)"
        )

    return _spec_str({_KEYS.get(k, k.replace("_", "-")): v for k, v in spec.items()})


cdef const eckit_geo_search.Grid* _grid_ptr(grid) except NULL:
    # eckit.geo.Grid is defined in another extension module
    return <const eckit_geo_search.Grid*>PyCapsule_GetPointer(
        grid._capsule(), "eckit::geo::Grid"
    )


cdef eckit_geo_search.Point _point(point, height) except *:
    cdef vector[double] c = [float(v) for v in point]
    if height is not None:
        c.push_back(float(height))
    return eckit_geo_search.make_point(c, height is not None)


def _arrays(*coordinates):
    import numpy as np

    arrays = [np.ascontiguousarray(c, dtype=np.float64).ravel() for c in coordinates]
    if any(a.size != arrays[0].size for a in arrays):
        raise ValueError("Search: coordinates of different sizes")
    return arrays


cdef const double* _data(const double[::1] a) noexcept nogil:
    return &a[0] if a.shape[0] > 0 else NULL


cdef _indices(const vector[size_t]& v):
    try:
        import numpy as np
    except ImportError:  # numpy is optional
        return list(v)

    out = np.empty(v.size(), dtype=np.intp)
    cdef Py_ssize_t[::1] array = out
    if v.size() > 0:
        memcpy(&array[0], v.data(), v.size() * sizeof(size_t))
    return out


cdef _doubles(const vector[double]& v):
    try:
        import numpy as np
    except ImportError:  # numpy is optional
        return list(v)

    out = np.empty(v.size(), dtype=np.float64)
    cdef double[::1] array = out
    if v.size() > 0:
        memcpy(&array[0], v.data(), v.size() * sizeof(double))
    return out


cdef _result(const eckit_geo_search.Result& r, bint distance):
    return (_indices(r.index), _doubles(r.distance)) if distance else _indices(r.index)


cdef _nn(const eckit_geo_search.Result& r, bint distance):
    cdef Py_ssize_t index = r.index[0]
    if distance:
        return index, r.distance[0]
    return index


cdef _csr(eckit_geo_search.CSR& csr, str order, distance_floor):
    try:
        import numpy as np
        from scipy.sparse import csr_array
    except ImportError as e:
        raise ImportError("Search: CSR searches require numpy and scipy") from e

    offsets = np.empty(csr.offsets.size(), dtype=np.intc)
    indices = np.empty(csr.size(), dtype=np.intc)
    distances = np.empty(csr.size(), dtype=np.float64)

    cdef int[::1] o = offsets
    cdef int[::1] i = indices
    cdef double[::1] d = distances
    memcpy(&o[0], csr.offsets.data(), csr.offsets.size() * sizeof(int))
    if csr.size() > 0:
        memcpy(&i[0], csr.indices.data(), csr.size() * sizeof(int))
        memcpy(&d[0], csr.distances.data(), csr.size() * sizeof(double))

    # scipy drops explicit zeros in some operations, raising zero distances keeps these
    # neighbours
    if distance_floor is not None:
        np.maximum(distances, distance_floor, out=distances)

    a = csr_array((distances, indices, offsets), shape=(csr.rows(), csr.cols))
    a.has_sorted_indices = order == "index"
    return a


cdef bint _by_index(str order) except -1:
    if order not in ("distance", "index"):
        raise ValueError(f"Search: order '{order}', expected 'distance' or 'index'")
    return order == "index"


cdef class Search:
    """Search on the points of a grid, following k-d tree algorithms.

    Points are (lon, lat) or geocentric (x, y, z); results are indices of the points
    (and distances, if asked), by increasing distance (chord length, in metres on the
    Earth), then index. Points with heights are searched in a space distorted by a
    height scale factor s (height_scale_factor, default 1): distances are as
    sqrt(horizontal^2 + (s dh)^2), so neighbours at similar heights are nearer.

    Searches of a grid share a k-d tree, cached by grid and k-d tree configuration
    within the cache capacity (see eckit.geo.cache); k, radius, ... are per search.
    Searches of many points (csr_search_*) release the GIL, and run on threads if
    eckit has OpenMP (ECKIT_GEO_SEARCH_NUM_THREADS). They query in Morton order, unless
    query_order="input" (if consecutive queries are already near each other, e.g. grid
    points).
    """
    cdef eckit_geo_search.SearchHandle* _handle

    def __cinit__(self, grid=None, **spec):
        self._handle = NULL
        if grid is None:
            return  # internal use only (from_xyz, configured)

        cdef const eckit_geo_search.Grid* ptr = _grid_ptr(grid)
        self._handle = eckit_geo_search.search_get(dereference(ptr), _spec(spec))

    def __dealloc__(self):
        if self._handle != NULL:
            del self._handle

    @staticmethod
    def from_xyz(x, y, z, **spec) -> Search:
        """Search on geocentric (x, y, z) points, the k-d tree cached by the points (not
        in the process, as searches of a grid)."""
        x, y, z = _arrays(x, y, z)
        cdef const double[::1] xv = x
        cdef const double[::1] yv = y
        cdef const double[::1] zv = z
        cdef string s = _spec(spec)
        cdef size_t n = xv.shape[0]

        cdef eckit_geo_search.SearchHandle* handle
        with nogil:
            handle = eckit_geo_search.search_from_xyz(
                n, _data(xv), _data(yv), _data(zv), s
            )

        cdef Search search = Search.__new__(Search)
        search._handle = handle
        return search

    @staticmethod
    def estimate(size: int) -> dict:
        """Memory a search of size points would use (bytes), in the process and
        shared."""
        usage = eckit_geo_search.search_estimate(size)
        return dict(zip(("process", "shared"), usage))

    def configured(self, **spec) -> Search:
        """Another behaviour (search, k, radius, ...), sharing the k-d tree."""
        cdef Search search = Search.__new__(Search)
        search._handle = eckit_geo_search.search_configured(
            dereference(self._handle), _spec(spec)
        )
        return search

    def search(self, point, height=None, distance: bool = False):
        """Search with the configured behaviour (nearest neighbour by default).

        A point is (lon, lat) or (x, y, z), or (lon, lat) and a height (of searches of
        points with heights). Returns the indices of the neighbours, and their distances
        if distance=True.
        """
        cdef eckit_geo_search.Point p = _point(point, height)
        return _result(
            eckit_geo_search.search_point(dereference(self._handle), p), distance
        )

    def search_nn(self, point, height=None, distance: bool = False):
        cdef eckit_geo_search.Point p = _point(point, height)
        return _nn(eckit_geo_search.search_nn(dereference(self._handle), p), distance)

    def search_knn(self, point, k, height=None, distance: bool = False):
        cdef eckit_geo_search.Point p = _point(point, height)
        return _result(
            eckit_geo_search.search_knn(dereference(self._handle), p, k), distance
        )

    def search_radius(self, point, radius, height=None, distance: bool = False):
        cdef eckit_geo_search.Point p = _point(point, height)
        return _result(
            eckit_geo_search.search_radius(dereference(self._handle), p, radius),
            distance,
        )

    def search_knn_or_radius(
        self, point, k, radius, height=None, distance: bool = False
    ):
        """Union of the k nearest neighbours and the neighbours within radius."""
        cdef eckit_geo_search.Point p = _point(point, height)
        return _result(
            eckit_geo_search.search_knn_or_radius(
                dereference(self._handle), p, k, radius
            ),
            distance,
        )

    def search_knn_and_radius(
        self, point, k, radius, height=None, distance: bool = False
    ):
        """Intersection of the k nearest neighbours and the neighbours within radius."""
        cdef eckit_geo_search.Point p = _point(point, height)
        return _result(
            eckit_geo_search.search_knn_and_radius(
                dereference(self._handle), p, k, radius
            ),
            distance,
        )

    def search_knn_or_same_radius(
        self, point, k, tolerance=1.0, height=None, distance: bool = False
    ):
        """k nearest neighbours or, if all as near (within tolerance), all the points
        as near (e.g. at a pole)."""
        cdef eckit_geo_search.Point p = _point(point, height)
        return _result(
            eckit_geo_search.search_knn_or_same_radius(
                dereference(self._handle), p, k, tolerance
            ),
            distance,
        )

    def csr_search_xyz(self, x, y, z, order="distance", distance_floor=1e-6):
        """Search with the configured behaviour, many (x, y, z) points.

        Returns a scipy.sparse.csr_array of distances: row i (point i) has the
        neighbours as columns (point indices), by order 'distance' (increasing
        distance, then index; scipy operations sorting indices lose it) or 'index'.
        Distances are raised to distance_floor (in the search distance unit), as scipy
        drops explicit zeros in some operations; None keeps zeros.

        Of search="knn", every row has min(k, len(search)) neighbours, so
        indices.reshape(-1, k) and data.reshape(-1, k) are (n, k) arrays as
        scipy.spatial.KDTree.query.
        """
        cdef bint by_index = _by_index(order)
        x, y, z = _arrays(x, y, z)
        cdef const double[::1] xv = x
        cdef const double[::1] yv = y
        cdef const double[::1] zv = z
        cdef size_t n = xv.shape[0]

        cdef eckit_geo_search.CSR csr
        with nogil:
            csr = eckit_geo_search.csr_search_xyz(
                dereference(self._handle), n, _data(xv), _data(yv), _data(zv), by_index
            )
        return _csr(csr, order, distance_floor)

    def csr_search_ll(self, lon, lat, order="distance", distance_floor=1e-6):
        """Search with the configured behaviour, many (lon, lat) points (see
        csr_search_xyz)."""
        cdef bint by_index = _by_index(order)
        lon, lat = _arrays(lon, lat)
        cdef const double[::1] lonv = lon
        cdef const double[::1] latv = lat
        cdef size_t n = lonv.shape[0]

        cdef eckit_geo_search.CSR csr
        with nogil:
            csr = eckit_geo_search.csr_search_ll(
                dereference(self._handle), n, _data(lonv), _data(latv), by_index
            )
        return _csr(csr, order, distance_floor)

    def csr_search_llh(self, lon, lat, height, order="distance", distance_floor=1e-6):
        """Search with the configured behaviour, many (lon, lat, height) points, of
        searches of points with heights (see query_coordinates, and csr_search_xyz)."""
        cdef bint by_index = _by_index(order)
        lon, lat, height = _arrays(lon, lat, height)
        cdef const double[::1] lonv = lon
        cdef const double[::1] latv = lat
        cdef const double[::1] hv = height
        cdef size_t n = lonv.shape[0]

        cdef eckit_geo_search.CSR csr
        with nogil:
            csr = eckit_geo_search.csr_search_llh(
                dereference(self._handle),
                n,
                _data(lonv),
                _data(latv),
                _data(hv),
                by_index,
            )
        return _csr(csr, order, distance_floor)

    @property
    def footprint(self) -> dict:
        """Memory used (bytes), in the process and shared (shared memory, page
        cache)."""
        usage = eckit_geo_search.search_footprint(dereference(self._handle))
        return dict(zip(("process", "shared"), usage))

    def __len__(self) -> int:
        return eckit_geo_search.search_size(dereference(self._handle))

    def __repr__(self) -> str:
        return eckit_geo_search.search_str(dereference(self._handle))
