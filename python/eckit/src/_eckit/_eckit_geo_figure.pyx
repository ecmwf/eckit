# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0


cimport eckit_geo_area
cimport eckit_geo_figure
from cpython.pycapsule cimport PyCapsule_GetPointer
from cython.operator cimport dereference

from eckit.geo._eckit_geo import _spec_str
from eckit.geo._eckit_geo_area import BoundingBox


cdef class Figure:
    cdef const eckit_geo_figure.Figure* _figure
    cdef bint _owned
    cdef object _owner  # keeps the object owning a borrowed _figure alive

    def __dealloc__(self):
        if self._owned and self._figure != NULL:
            del self._figure

    def __cinit__(self, spec = None, **kwargs):
        self._figure = NULL
        self._owned = False
        self._owner = None
        if spec is None and not kwargs:
            return  # internal use only (borrowed pointer, see _borrow)
        assert bool(spec) != bool(kwargs)

        if kwargs or isinstance(spec, dict):
            spec = _spec_str(kwargs if kwargs else spec)

        assert isinstance(spec, str)
        self._figure = eckit_geo_figure.FigureFactory.make_from_string(spec)
        self._owned = True

    @staticmethod
    def _borrow(capsule, owner) -> Figure:
        """Wrap a C++ figure owned by `owner` (no ownership transfer)."""
        cdef Figure figure = Figure.__new__(Figure)
        figure._figure = <const eckit_geo_figure.Figure*>PyCapsule_GetPointer(
            capsule, "eckit::geo::Figure"
        )
        figure._owner = owner
        return figure

    def __eq__(self, other) -> bool:
        if not isinstance(other, Figure):
            return NotImplemented
        return dereference(self._figure) == dereference((<Figure>other)._figure)

    def area(self, bbox = None) -> float:
        if bbox is None:
            return self._figure.area()
        if not isinstance(bbox, BoundingBox):
            raise TypeError("bbox must be a BoundingBox")
        return self._figure.area(dereference(
            <const eckit_geo_area.BoundingBox*>PyCapsule_GetPointer(
                bbox._capsule(), "eckit::geo::area::BoundingBox"
            )
        ))

    @property
    def R(self) -> float:
        return self._figure.R()

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
        from json import loads
        return loads(self.spec_str)

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
