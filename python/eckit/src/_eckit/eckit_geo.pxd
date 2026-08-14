# (C) Copyright 1996- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.


from libcpp cimport bool
from libcpp.memory cimport unique_ptr
from libcpp.string cimport string
from libcpp.utility cimport pair
from libcpp.vector cimport vector


cdef extern from "eckit/geo/LibEcKitGeo.h" namespace "eckit":
    cdef cppclass LibEcKitGeo:
        @staticmethod
        LibEcKitGeo& instance()

        @staticmethod
        void purgeCacheDir()

        string version()
        string gitsha1(unsigned int n)  # n=40 for full sha1


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


cdef extern from "eckit/geo/Figure.h" namespace "eckit::geo":
    cdef cppclass Figure:
        double a() const
        double b() const
        double area() const
        double area(const BoundingBox&) const
        string spec_str() const
        string proj_str() const
        bool spherical() const
        double eccentricity() const
        double flattening() const

    cdef cppclass FigureFactory:
        @staticmethod
        Figure* make_from_string(const string) except +


cdef extern from "eckit/geo/Range.h" namespace "eckit::geo":
    cdef cppclass Range:
        vector[double] values() const


cdef extern from "eckit/geo/Grid.h" namespace "eckit::geo":
    cdef cppclass Grid:
        string spec_str() const
        string catalog_str() const
        string type() const
        string uid() const
        string order() const

        pair[vector[double], vector[double]] to_latlons() const

        vector[size_t] shape() const
        size_t size() const
        const BoundingBox& boundingBox() const
        const Range& x() const
        const Range& y() const
        const Range& lon() const
        const Range& lat() const

    cdef cppclass GridFactory:
        @staticmethod
        const Grid* make_from_string(const string) except +


cdef extern from * namespace "eckit::geo::python":
    """
    #include <vector>

    #include "eckit/geo/area/BoundingBox.h"
    #include "eckit/geo/Figure.h"
    #include "eckit/geo/Grid.h"
    #include "eckit/geo/PointLonLat.h"

    namespace eckit::geo::python {

    using v = std::vector<double>;

    inline v grid_x_values(const Grid& grid) { return grid.x().values(); }
    inline v grid_y_values(const Grid& grid) { return grid.y().values(); }
    inline v grid_lon_values(const Grid& grid) { return grid.lon().values(); }
    inline v grid_lat_values(const Grid& grid) { return grid.lat().values(); }

    inline bool bbox_intersects(const area::BoundingBox& lhs, area::BoundingBox& rhs) {
        return lhs.intersects(rhs);
    }

    inline bool bbox_contains_lonlat(
        const area::BoundingBox& bbox, double lon, double lat
    ) {
        return bbox.contains(PointLonLat{lon, lat});
    }

    inline double figure_R(const Figure& figure) { return figure.R(); }

    }
    """
    vector[double] grid_x_values(const Grid& grid) except +
    vector[double] grid_y_values(const Grid& grid) except +
    vector[double] grid_lon_values(const Grid& grid) except +
    vector[double] grid_lat_values(const Grid& grid) except +

    bint bbox_intersects(const BoundingBox& lhs, BoundingBox& rhs) except +
    bint bbox_contains_lonlat(const BoundingBox& bbox, double lon, double lat) except +
    double figure_R(const Figure& figure) except +
