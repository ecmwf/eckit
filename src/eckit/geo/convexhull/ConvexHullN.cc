/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/geo/convexhull/ConvexHullN.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/util/Qhull.h"


namespace eckit::geo::convexhull {


std::vector<std::vector<double>> ConvexHullN::list_vertices() const {
    NOTIMP;
}


std::vector<std::vector<size_t>> ConvexHullN::list_facets() const {
    NOTIMP;
}


std::vector<ConvexHull::Triangle> ConvexHullN::list_triangles() const {
    NOTIMP;
}


ConvexHullN::ConvexHullN(size_t N, const std::vector<double>& coord) {
    // util::Qhull q(N, coord, "QJ");
}


}  // namespace eckit::geo::convexhull
