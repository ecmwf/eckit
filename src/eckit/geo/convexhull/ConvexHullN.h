/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#pragma once

#include "eckit/geo/ConvexHull.h"

#include <memory>

#include "eckit/geo/util/Qhull.h"


namespace eckit::geo::convexhull {


template <size_t N>
class ConvexHullN : public ConvexHull {
public:
    // -- Types

    using coord_t = util::Qhull::coord_t;

    // -- Constructors

    explicit ConvexHullN(const coord_t& coord) :
        qhull_(N, coord, "QJ") {}

    explicit ConvexHullN(const std::vector<std::vector<double>>& coord_v) :
        ConvexHullN(convert_vector_v(coord_v)) {}

    explicit ConvexHullN(const std::vector<std::array<double, N>>& coord_a) :
        ConvexHullN(convert_vector_a(coord_a)) {}

    // -- Methods

    std::vector<size_t> list_vertices() const override { return qhull_.list_vertices(); }

    std::vector<std::vector<size_t>> list_facets() const override { return qhull_.list_facets(); }

    std::vector<Triangle> list_triangles() const override { return qhull_.list_triangles(); }

private:
    // -- Methods

    static coord_t convert_vector_v(const std::vector<std::vector<double>>& coord_v) {
        coord_t coord;
        coord.reserve(N * coord_v.size());

        for (const auto& v : coord_v) {
            ASSERT(N == v.size());
            for (int i = 0; i < N; ++i) {
                coord.emplace_back(v[i]);
            }
        }

        return coord;
    }

    static coord_t convert_vector_a(const std::vector<std::array<double, N>>& coord_a) {
        coord_t coord;
        coord.reserve(N * coord_a.size());
        for (const auto& a : coord_a) {
            for (int i = 0; i < N; ++i) {
                coord.emplace_back(a[i]);
            }
        }
        return coord;
    }

    // -- Members

    util::Qhull qhull_;
};


}  // namespace eckit::geo::convexhull
