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


#include "eckit/geometry/grid/ReducedLL.h"

#include <algorithm>
#include <cmath>
#include <sstream>

#include "eckit/geometry/Domain.h"
#include "eckit/geometry/Projection.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/types/Fraction.h"


namespace eckit::geometry::grid {

static bool checkPl(const std::vector<long>& pl) {
    ASSERT(!pl.empty());
    return *std::min_element(pl.begin(), pl.end()) >= 2;
}

ReducedLL::ReducedLL(const Configuration& config) :
    Grid(config) {
    ASSERT(config.get("pl", pl_));
    checkPl(pl_);

    size_t Nj = 0;
    ASSERT(config.get("Nj", Nj));
    ASSERT(Nj == pl_.size());
}

ReducedLL::~ReducedLL() = default;

size_t ReducedLL::size() const {
    size_t total = 0;
    for (const auto& j : pl_) {
        total += size_t(j);
    }
    return total;
}

bool ReducedLL::isPeriodicWestEast() const {
    ASSERT(!pl_.empty());

    auto maxpl = *std::max_element(pl_.begin(), pl_.end());
    ASSERT(maxpl >= 2);

    // if range West-East is within one increment (or greater than) 360 degree
    const Fraction inc(360, maxpl);
    return bbox().east() - bbox().west() + inc >= GLOBE;
}

bool ReducedLL::includesNorthPole() const {
    return bbox().north() == NORTH_POLE;
}

bool ReducedLL::includesSouthPole() const {
    return bbox().south() == SOUTH_POLE;
}

static const GridRegisterType<ReducedLL> reducedLL("reduced_ll");


}  // namespace eckit::geometry::grid
