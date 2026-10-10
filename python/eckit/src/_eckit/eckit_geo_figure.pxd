# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0


from libcpp cimport bool
from libcpp.string cimport string

from eckit_geo_area cimport BoundingBox


cdef extern from "eckit/geo/Figure.h" namespace "eckit::geo":
    cdef cppclass Figure:
        double R() except +  # spherical figures only
        double a() const
        double b() const
        double area() const
        double area(const BoundingBox&) const
        string spec_str() const
        string proj_str() const
        bool spherical() const
        double eccentricity() const
        double flattening() const
        bool operator==(const Figure&) const

    cdef cppclass FigureFactory:
        @staticmethod
        Figure* make_from_string(const string) except +
