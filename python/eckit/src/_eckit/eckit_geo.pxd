# (C) Copyright 2025- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.


from libcpp.string cimport string
from libcpp.utility cimport pair
from libcpp.vector cimport vector


cdef extern from "eckit/geo/Area.h" namespace "eckit::geo":
    cdef cppclass Area:
        string spec_str() const
        string type() const

    cdef cppclass AreaFactory:
        @staticmethod
        const Area* make_from_string(const string) except +

cdef extern from "eckit/geo/area/BoundingBox.h" namespace "eckit::geo::area":
    cdef cppclass BoundingBox(Area):
        double north() const
        double west() const
        double south() const
        double east() const


cdef extern from "eckit/geo/Grid.h" namespace "eckit::geo":
    cdef cppclass Grid:
        pair[vector[double], vector[double]] to_latlons() const
        string spec_str() const
        string type() const
        string uid() const
        vector[size_t] shape() const
        size_t size() const
        const BoundingBox& boundingBox() const

    cdef cppclass GridFactory:
        @staticmethod
        const Grid* make_from_string(const string) except +
