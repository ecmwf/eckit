# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0


from libcpp cimport bool
from libcpp.memory cimport unique_ptr
from libcpp.string cimport string


cdef extern from "eckit/geo/Area.h" namespace "eckit::geo":
    cdef cppclass Area:
        string spec_str() const
        string type() const

    cdef cppclass AreaFactory:
        @staticmethod
        const Area* make_from_string(const string) except +


cdef extern from "eckit/geo/area/BoundingBox.h" namespace "eckit::geo::area":
    cdef cppclass BoundingBox(Area):
        BoundingBox(double north, double west, double south, double east) except +

        double north() const
        double west() const
        double south() const
        double east() const

        bool intersects(BoundingBox&) const
        bool contains(const BoundingBox&) const
        bool is_global "global"() const
        bool periodic() const
        bool empty() const
        double area() const

        @staticmethod
        unique_ptr[BoundingBox] make_from_area(
            double n, double w, double s, double e
        ) except +


cdef extern from * namespace "eckit::geo::python":
    """
    #include "eckit/geo/area/BoundingBox.h"
    #include "eckit/geo/Point.h"

    namespace eckit::geo::python {

    inline bool bbox_intersects(const area::BoundingBox& lhs, area::BoundingBox& rhs) {
        return lhs.intersects(rhs);
    }

    inline bool bbox_contains_lonlat(
        const area::BoundingBox& bbox, double lon, double lat
    ) {
        return bbox.contains(PointLonLat{lon, lat});
    }

    }
    """
    bint bbox_intersects(const BoundingBox& lhs, BoundingBox& rhs) except +
    bint bbox_contains_lonlat(const BoundingBox& bbox, double lon, double lat) except +
