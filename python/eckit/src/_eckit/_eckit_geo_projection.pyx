# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0


cimport eckit_geo_projection
from cpython.pycapsule cimport PyCapsule_GetPointer
from cpython.pycapsule cimport PyCapsule_New
from cython.operator cimport dereference
from libc.string cimport memcpy
from libcpp.vector cimport vector

from eckit.geo._eckit_geo import _spec_str
from eckit.geo._eckit_geo_figure import Figure


cdef class Projection:
    cdef const eckit_geo_projection.Projection* _projection
    cdef bint _owned
    cdef object _owner  # keeps the object owning a borrowed _projection alive

    def __dealloc__(self):
        if self._owned and self._projection != NULL:
            del self._projection

    def __cinit__(self, spec = None, **kwargs):
        self._projection = NULL
        self._owned = False
        self._owner = None
        if spec is None and not kwargs:
            return  # internal use only (borrowed pointer, see _borrow)
        assert bool(spec) != bool(kwargs)

        if kwargs or isinstance(spec, dict):
            spec = _spec_str(kwargs if kwargs else spec)

        assert isinstance(spec, str)
        self._projection = eckit_geo_projection.ProjectionFactory.make_from_string(spec)
        self._owned = True

    @staticmethod
    def _borrow(capsule, owner) -> Projection:
        """Wrap a C++ projection owned by `owner` (no ownership transfer)."""
        cdef Projection projection = Projection.__new__(Projection)
        projection._projection = (
            <const eckit_geo_projection.Projection*>PyCapsule_GetPointer(
                capsule, "eckit::geo::Projection"
            )
        )
        projection._owner = owner
        return projection

    def __eq__(self, other) -> bool:
        if not isinstance(other, Projection):
            return NotImplemented
        return (dereference(self._projection) ==
                dereference((<Projection>other)._projection))

    cdef tuple _project(self, bint forward, tuple ordered, dict named):
        names = tuple(self._projection.source_point_coordinates() if forward else
                      self._projection.target_point_coordinates())

        # coordinates, ordered or named (not mixed), as the point coordinates
        if (ordered and named) or (
            set(named) != set(names) if named else len(ordered) != len(names)
        ):
            raise TypeError(
                f"Projection: expected coordinates {names}, ordered or named"
            )
        coordinates = [named[name] for name in names] if named else ordered

        # to vectors: all scalars (one point) or 1-d array-likes (many points)
        cdef vector[vector[double]] v = vector[vector[double]](3)
        cdef const double[::1] buffer
        scalars = set()
        for k, c in enumerate(coordinates):
            try:
                len(c)
                scalars.add(False)
            except TypeError:
                scalars.add(True)
                v[k].push_back(c)
                continue

            if getattr(c, "ndim", 1) != 1:
                raise ValueError("Projection: coordinate arrays are 1-dimensional")
            try:
                buffer = c  # contiguous float64 buffers (eg. numpy) copied directly
                if buffer.shape[0] > 0:
                    v[k].assign(&buffer[0], &buffer[0] + buffer.shape[0])
            except (TypeError, ValueError):
                v[k] = [float(x) for x in c]

        if len(scalars) > 1:
            raise ValueError(
                "Projection: coordinates are either all scalars or all 1-d array-likes"
            )

        cdef vector[vector[double]] w = (
            self._projection.fwd(v[0], v[1], v[2]) if forward else
            self._projection.inv(v[0], v[1], v[2])
        )

        # from vectors: floats (one point), numpy arrays or lists (many points)
        if True in scalars:
            return tuple(c[0] for c in w)

        try:
            import numpy as np
        except ImportError:  # numpy is optional
            return tuple(list(c) for c in w)

        cdef double[::1] array
        out = tuple(np.empty(c.size()) for c in w)
        for k in range(w.size()):
            array = out[k]
            if w[k].size() > 0:
                memcpy(&array[0], w[k].data(), w[k].size() * sizeof(double))
        return out

    def fwd(self, *coordinates, **named) -> tuple:
        """Project points from source_point_coordinates to target_point_coordinates.

        Arguments are either all scalars (one point) or all 1-d array-likes of the same
        length (many points).

        Returns a tuple as the target_point_coordinates (in size and order), of floats
        (one point) or of 1-d numpy float64 arrays (lists without numpy). Points failing
        to project result in NaN.

        Warning: coordinates are copied (to/from C++ vectors).
        """
        return self._project(True, coordinates, named)

    def inv(self, *coordinates, **named) -> tuple:
        """Project points from target_point_coordinates back to
        source_point_coordinates (see fwd, with source and target swapped)."""
        return self._project(False, coordinates, named)

    @property
    def source_point_coordinates(self) -> tuple:
        """Coordinate names (in order) fwd() takes and inv() returns, as defined by
        eckit::geo (point_coordinates)."""
        return tuple(self._projection.source_point_coordinates())

    @property
    def target_point_coordinates(self) -> tuple:
        """Coordinates fwd() returns and inv() takes (see source_point_coordinates)."""
        return tuple(self._projection.target_point_coordinates())

    @property
    def figure(self) -> Figure:
        return Figure._borrow(
            PyCapsule_New(
                <void*>&self._projection.figure(), "eckit::geo::Figure", NULL
            ),
            self,
        )

    @property
    def spec_str(self) -> str:
        return self._projection.spec_str()

    @property
    def spec(self) -> dict:
        from json import loads
        return loads(self.spec_str)

    @property
    def proj_str(self) -> str:
        return self._projection.proj_str()

    @property
    def type(self) -> str:
        return self._projection.type()
