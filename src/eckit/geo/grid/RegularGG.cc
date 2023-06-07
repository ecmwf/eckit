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


#include "eckit/geo/grid/RegularGG.h"

#include <algorithm>
#include <ostream>
#include <utility>

#include "eckit/geo/grid/GaussianIterator.h"


namespace eckit::geo::grid {


RegularGG::RegularGG(const Configuration& config) :
    Regular(config) {}


RegularGG::RegularGG(size_t N, const BoundingBox& bbox, double angularPrecision) :
    Regular(N, bbox, angularPrecision) {}


void RegularGG::print(std::ostream& out) const {
    out << "RegularGG["
           "N="
        << N_ << ",Ni=" << Ni_ << ",Nj=" << Nj_ << ",bbox=" << bbox() << "]";
}


Iterator* RegularGG::iterator() const {
    std::vector<long> pl(N_ * 2, long(4 * N_));
    return new GaussianIterator(latitudes(), std::move(pl), bbox(), N_, Nj_, k_);
}


static const GridBuilder<RegularGG> reducedGG("regular_gg");


}  // namespace eckit::geo::grid
