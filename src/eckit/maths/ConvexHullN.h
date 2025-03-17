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

#include <algorithm>

#include "eckit/maths/ConvexHull.h"


namespace eckit::maths {


template <size_t N>
class ConvexHullN : public ConvexHull {
public:

    explicit ConvexHullN(const ConvexHull::coord_t& coord, const std::string& qhull_command = Qhull::COMMAND_DEFAULT) :
        qhull_(N, coord, qhull_command) {}

    explicit ConvexHullN(const std::vector<std::vector<double>>& coord_v,
                         const std::string& qhull_command = Qhull::COMMAND_DEFAULT) :
        ConvexHullN(convert_vector_v(coord_v), qhull_command) {}

    explicit ConvexHullN(const std::vector<std::array<double, N>>& coord_a,
                         const std::string& qhull_command = Qhull::COMMAND_DEFAULT) :
        ConvexHullN(convert_vector_a(coord_a), qhull_command) {}

    std::vector<size_t> list_vertices() const override { return qhull_.list_vertices(); }
    std::vector<std::vector<size_t>> list_facets() const override { return qhull_.list_facets(); }

    ConvexHull::facets_n_t facets_n() const override { return qhull_.facets_n(); }
    std::vector<size_t> facets(size_t n) const override { return qhull_.facets(n); }

private:

    static coord_t convert_vector_v(const std::vector<std::vector<double>>& coord_v) {
        coord_t coord;
        coord.reserve(N * coord_v.size());
        for (const auto& v : coord_v) {
            ASSERT(N == v.size());
            std::for_each_n(v.begin(), N, [&coord](auto x) { coord.emplace_back(x); });
        }
        return coord;
    }

    static coord_t convert_vector_a(const std::vector<std::array<double, N>>& coord_a) {
        coord_t coord;
        coord.reserve(N * coord_a.size());
        for (const auto& a : coord_a) {
            std::for_each_n(a.begin(), N, [&coord](auto x) { coord.emplace_back(x); });
        }
        return coord;
    }

    Qhull qhull_;
};


}  // namespace eckit::maths
