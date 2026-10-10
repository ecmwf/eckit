# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0


from libcpp cimport bool
from libcpp.string cimport string
from libcpp.vector cimport vector

from eckit_geo_figure cimport Figure


cdef extern from "eckit/geo/Projection.h" namespace "eckit::geo":
    cdef cppclass Projection:
        string spec_str() const
        string proj_str() const
        const string& type() const
        const vector[string]& source_point_coordinates() const
        const vector[string]& target_point_coordinates() const
        vector[vector[double]] fwd(
            const vector[double]& v1, const vector[double]& v2, const vector[double]& v3
        ) except +
        vector[vector[double]] inv(
            const vector[double]& v1, const vector[double]& v2, const vector[double]& v3
        ) except +
        const Figure& figure() except +
        bool operator==(const Projection&) const

    cdef cppclass ProjectionFactory:
        @staticmethod
        const Projection* make_from_string(const string) except +
