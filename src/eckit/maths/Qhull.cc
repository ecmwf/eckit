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

#include <set>
#include <sstream>

#include "eckit/log/Log.h"
#include "eckit/maths/ConvexHull.h"

#include "libqhullcpp/Qhull.h"
#include "libqhullcpp/QhullFacetList.h"
#include "libqhullcpp/QhullVertexSet.h"


namespace eckit::maths {


const char* Qhull::COMMAND_DEFAULT = "QJ";


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
        static const std::set<int> DIMENSION_ERROR{6050};

        static const std::set<int> INPUT_ERROR{6010, 6013, 6019, 6020, 6021, 6022, 6023, 6033, 6067, 6070,
                                               6072, 6073, 6074, 6075, 6077, 6078, 6150, 6151, 6152, 6153,
                                               6203, 6204, 6214, 6220, 6221, 6222, 6223, 6229, 6411, 6412};

        static const std::set<int> OPTION_ERROR{6006, 6029, 6035, 6036, 6037, 6041, 6044, 6045, 6046,
                                                6047, 6048, 6049, 6051, 6053, 6054, 6055, 6056, 6057,
                                                6058, 6059, 6215, 6362, 6363, 6364, 6365, 6375};

        static const std::set<int> PRECISION_ERROR{6012, 6109, 6110, 6111, 6112, 6113, 6114, 6115, 6116,
                                                   6117, 6118, 6136, 6154, 6239, 6240, 6297, 6298, 6347,
                                                   6348, 6354, 6379, 6380, 6417, 6418, 6422};

        static const std::set<int> TOPOLOGY_ERROR{6001, 6107, 6155, 6168, 6170, 6208, 6227, 6260,
                                                  6271, 6356, 6361, 6381, 6391, 6420, 6425};

        auto is = [](int err, const auto& set) { return set.find(err) != set.end(); };

        is(e.errorCode(), DIMENSION_ERROR)   ? throw ConvexHull::DimensionError(err.str(), e.errorCode(), command)
        : is(e.errorCode(), INPUT_ERROR)     ? throw ConvexHull::InputError(err.str(), e.errorCode(), command)
        : is(e.errorCode(), OPTION_ERROR)    ? throw ConvexHull::OptionError(err.str(), e.errorCode(), command)
        : is(e.errorCode(), PRECISION_ERROR) ? throw ConvexHull::PrecisionError(err.str(), e.errorCode(), command)
        : is(e.errorCode(), TOPOLOGY_ERROR)  ? throw ConvexHull::TopologyError(err.str(), e.errorCode(), command)
                                             : throw ConvexHull::Exception(err.str(), e.errorCode(), command);
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


std::vector<std::vector<size_t>> Qhull::list_facets() const {
    std::vector<std::vector<size_t>> facets;
    facets.reserve(qh_->facetCount());

    for (const auto& facet : qh_->facetList()) {
        const auto vertices = facet.vertices();

        std::vector<size_t> f;
        f.reserve(vertices.size());

        for (const auto& vertex : vertices) {
            f.emplace_back(vertex.point().id());
        }

        facets.emplace_back(std::move(f));
    }

    return facets;
}


Qhull::facets_n_t Qhull::facets_n() const {
    facets_n_t counts;
    for (const auto& facet : qh_->facetList()) {
        counts[facet.vertices().size()]++;
    }
    return counts;
}


std::vector<size_t> Qhull::facets(size_t n) const {
    ASSERT(0 < n);

    std::vector<size_t> facets;
    facets.reserve(qh_->facetCount() * n);

    for (const auto& facet : qh_->facetList()) {
        if (const auto vertices = facet.vertices(); vertices.size() == n) {
            for (const auto& vertex : vertices) {
                facets.emplace_back(vertex.point().id());
            }
        }
    }

    return facets;
}


}  // namespace eckit::maths
