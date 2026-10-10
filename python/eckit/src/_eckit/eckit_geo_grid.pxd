# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0


from libcpp.string cimport string
from libcpp.utility cimport pair
from libcpp.vector cimport vector

from eckit_geo_area cimport BoundingBox
from eckit_geo_figure cimport Figure
from eckit_geo_projection cimport Projection


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

        pair[vector[double], vector[double]] to_latlons() except +
        Grid* to_unstructured_ll(const string& name) except +

        vector[size_t] shape() const
        size_t size() const
        const BoundingBox& boundingBox() const
        const Projection& projection() except +
        const Figure& figure() except +
        const Range& x() const
        const Range& y() const
        const Range& lon() const
        const Range& lat() const

    cdef cppclass GridFactory:
        @staticmethod
        const Grid* make_from_string(const string) except +


cdef extern from "eckit/geo/grid/Unstructured.h" namespace "eckit::geo::grid":
    cdef cppclass Unstructured(Grid):
        Unstructured(
            const vector[double]& longitudes,
            const vector[double]& latitudes,
            const string& name,
        ) except +


cdef extern from * namespace "eckit::geo::python":
    """
    #include <vector>

    #include "eckit/geo/Grid.h"

    namespace eckit::geo::python {

    using v = std::vector<double>;

    inline v grid_x_values(const Grid& grid) { return grid.x().values(); }
    inline v grid_y_values(const Grid& grid) { return grid.y().values(); }
    inline v grid_lon_values(const Grid& grid) { return grid.lon().values(); }
    inline v grid_lat_values(const Grid& grid) { return grid.lat().values(); }

    }
    """
    vector[double] grid_x_values(const Grid& grid) except +
    vector[double] grid_y_values(const Grid& grid) except +
    vector[double] grid_lon_values(const Grid& grid) except +
    vector[double] grid_lat_values(const Grid& grid) except +
