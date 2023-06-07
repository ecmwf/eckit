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


#include "eckit/geo/grid/Octahedral.h"


namespace eckit::geo::grid {


Octahedral::Octahedral(size_t N, const BoundingBox& box, double angularPrecision) :
    Reduced(N, box, angularPrecision) {

    // adjust latitudes, longitudes and re-set bounding box
    auto n = bbox().north();
    auto s = bbox().south();
    correctSouthNorth(s, n);

    setNj(pls("O" + std::to_string(N_)), s, n);

    auto w = bbox().west();
    auto e = bbox().east();
    correctWestEast(w, e);

    bbox({n, w, s, e});
}


}  // namespace eckit::geo::grid
