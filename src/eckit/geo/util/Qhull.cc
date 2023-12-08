/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/geo/util/Qhull.h"

#include "eckit/exception/Exceptions.h"

#include "libqhullcpp/QhullFacetList.h"
#include "libqhullcpp/QhullVertexSet.h"


namespace eckit::geo::util {


Qhull::Qhull(size_t N, const coord_t& coord, const std::string& command) {
    ASSERT(0 < N && coord.size() % N == 0);

    auto pointDimension = static_cast<int>(N);
    auto pointCount     = static_cast<int>(coord.size() / N);

    qh_ = std::make_unique<decltype(qh_)::element_type>("", pointDimension, pointCount, coord.data(), command.c_str());
    ASSERT(qh_);
}


std::vector<size_t> Qhull::list_vertices() const {
    std::vector<size_t> vertices;
    vertices.reserve(qh_->vertexCount());

    for (const auto& vertex : qh_->vertexList()) {
        vertices.emplace_back(vertex.point().id());
    }

    return vertices;
}


std::vector<std::vector<size_t>> Qhull::list_facets() const {
    std::vector<std::vector<size_t>> facets;
    facets.reserve(qh_->facetCount());

    for (const auto& facet : qh_->facetList()) {
        const auto vertices = facet.vertices();

        std::vector<size_t> f;
        f.reserve(vertices.size());

        for (const auto& vertex : vertices) {
            f.push_back(static_cast<size_t>(vertex.point().id()));
        }

        facets.emplace_back(f);
    }

    return facets;
}


std::vector<ConvexHull::Triangle> Qhull::list_triangles() const {
    std::vector<Triangle> tri;
    tri.reserve(qh_->facetCount());

    for (const auto& facet : qh_->facetList()) {
        const auto vertices = facet.vertices();
        ASSERT(vertices.size() == 3);

        tri.emplace_back(Triangle{static_cast<Triangle::value_type>(vertices[0].point().id()),
                                  static_cast<Triangle::value_type>(vertices[1].point().id()),
                                  static_cast<Triangle::value_type>(vertices[2].point().id())});
    }

    return tri;
}


}  // namespace eckit::geo::util
