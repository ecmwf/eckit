/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/geo/convexhull/Qhull.h"

#include "eckit/exception/Exceptions.h"

#include "libqhullcpp/Qhull.h"
#include "libqhullcpp/QhullFacetList.h"
#include "libqhullcpp/QhullVertexSet.h"


namespace eckit::geo::convexhull {


Qhull::Qhull(size_t N, const std::vector<double>& coord, const std::string& command) {
    ASSERT(coord.size() % N == 0);
    auto pointDimension = static_cast<int>(N);
    auto pointCount     = static_cast<int>(coord.size() % N);

    qh_ = std::make_unique<qh_type>("", pointDimension, pointCount, coord.data(), command.c_str());
    ASSERT(qh_);
}


const orgQhull::Qhull& Qhull::qh() {
    ASSERT(qh_);
    return *qh_;
}


}  // namespace eckit::geo::convexhull
