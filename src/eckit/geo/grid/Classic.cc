/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/geo/grid/Classic.h"


namespace eckit::geo::grid {


Classic::Classic(size_t N, const BoundingBox& box, double angularPrecision) :
    Reduced(N, box, angularPrecision) {

    // adjust latitudes, longitudes and re-set bounding box
    auto n = box.north();
    auto s = box.south();
    correctSouthNorth(s, n);

    setNj(pls("N" + std::to_string(N_)), s, n);

    auto w = box.west();
    auto e = box.east();
    correctWestEast(w, e);

    bbox({n, w, s, e});
}


bool Classic::sameAs(const Grid& other) const {
    const auto* o = dynamic_cast<const Classic*>(&other);
    return (o != nullptr) && Reduced::sameAs(other);
}


}  // namespace eckit::geo::grid
