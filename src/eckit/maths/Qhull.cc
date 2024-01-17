/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/maths/Qhull.h"

#include <sstream>

#include "eckit/log/Log.h"

#include "libqhullcpp/Qhull.h"
#include "libqhullcpp/QhullFacetList.h"
#include "libqhullcpp/QhullVertexSet.h"


namespace eckit::maths {


Qhull::Qhull(size_t N, const coord_t& coord, const std::string& command) {
    ASSERT(0 < N && coord.size() % N == 0);

    auto pointDimension = static_cast<int>(N);
    auto pointCount     = static_cast<int>(coord.size() / N);

    qh_ = new orgQhull::Qhull;
    ASSERT(qh_ != nullptr);

    std::ostringstream err;
    qh_->setErrorStream(&err);
    qh_->setOutputStream(&Log::info());
    qh_->enableOutputStream();

    try {
        qh_->runQhull("", pointDimension, pointCount, coord.data(), command.c_str());
        ASSERT(qh_->qhullStatus() == 0);
    }
    catch (const orgQhull::QhullError& e) {
        throw Exception(err.str(), e.errorCode(), command);
    }
}


Qhull::~Qhull() {
    delete qh_;
}


std::vector<size_t> Qhull::list_vertices() const {
    std::vector<size_t> vertices;
    vertices.reserve(qh_->vertexCount());

    for (const auto& vertex : qh_->vertexList()) {
        vertices.emplace_back(vertex.point().id());
    }

    return vertices;
}


std::vector<std::vector<size_t>> Qhull::list_facets(size_t n) const {
    std::vector<std::vector<size_t>> facets;
    facets.reserve(qh_->facetCount());

    for (const auto& facet : qh_->facetList()) {
        if (const auto vertices = facet.vertices(); n == 0 || vertices.size() == n) {
            std::vector<size_t> f;
            f.reserve(vertices.size());

            for (const auto& vertex : vertices) {
                f.emplace_back(vertex.point().id());
            }

            facets.emplace_back(std::move(f));
        }
    }

    return facets;
}


}  // namespace eckit::maths
