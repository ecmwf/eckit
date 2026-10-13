# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0


from libcpp.string cimport string
from libcpp.utility cimport pair
from libcpp.vector cimport vector

from eckit_geo_area cimport Area
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
        vector[vector[double]] to_xyz() except +
        Grid* to_unstructured_ll(const string& name) except +

        vector[size_t] shape() const
        size_t size() const
        vector[size_t] reorder(const string&) except +
        const BoundingBox& boundingBox() const
        vector[size_t] crop(const Area&) except +
        Grid* make_grid_cropped(const Area&) except +
        Grid* make_grid_cropped_unstructured_ll(const Area&) except +
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
    #include <string>
    #include <type_traits>
    #include <variant>
    #include <vector>

    #include "eckit/geo/Exceptions.h"
    #include "eckit/geo/Grid.h"
    #include "eckit/geo/Point.h"

    namespace eckit::geo::python {

    using v = std::vector<double>;

    inline v grid_x_values(const Grid& grid) { return grid.x().values(); }
    inline v grid_y_values(const Grid& grid) { return grid.y().values(); }
    inline v grid_lon_values(const Grid& grid) { return grid.lon().values(); }
    inline v grid_lat_values(const Grid& grid) { return grid.lat().values(); }

    inline v point_values(const Point& p) {
        return std::visit(
            [](const auto& q) -> v {
                using P = std::decay_t<decltype(q)>;
                if constexpr (std::is_same_v<P, PointXY>) {
                    return {q.X(), q.Y()};
                }
                else if constexpr (std::is_same_v<P, PointXYZ>) {
                    return {q.X(), q.Y(), q.Z()};
                }
                else if constexpr (std::is_same_v<P, PointLonLat>) {
                    return {q.lon(), q.lat()};
                }
                else if constexpr (std::is_same_v<P, PointLonLatR>) {
                    return {q.lonr(), q.latr()};
                }
                else {
                    throw exception::GridError("Grid: invalid point", Here());
                }
            },
            p);
    }

    inline std::vector<v> grid_points(const Grid& grid) {
        // one vector per coordinate (see grid_point_coordinates)
        std::vector<v> points;
        for (const auto& p : grid) {
            const auto values = point_values(p);
            if (points.empty()) {
                points.assign(values.size(), {});
                for (auto& c : points) {
                    c.reserve(grid.size());
                }
            }
            for (size_t k = 0; k < values.size(); ++k) {
                points[k].emplace_back(values[k]);
            }
        }
        return points;
    }

    inline std::vector<std::string> grid_point_coordinates(const Grid& grid) {
        return std::visit(
            [](const auto& q) -> std::vector<std::string> {
                using P = std::decay_t<decltype(q)>;
                if constexpr (std::is_same_v<P, std::monostate>) {
                    throw exception::GridError("Grid: invalid point", Here());
                }
                else {
                    return point_coordinates<P>();
                }
            },
            grid.first_point());
    }

    }
    """
    vector[double] grid_x_values(const Grid& grid) except +
    vector[double] grid_y_values(const Grid& grid) except +
    vector[double] grid_lon_values(const Grid& grid) except +
    vector[double] grid_lat_values(const Grid& grid) except +
    vector[vector[double]] grid_points(const Grid& grid) except +
    vector[string] grid_point_coordinates(const Grid& grid) except +
