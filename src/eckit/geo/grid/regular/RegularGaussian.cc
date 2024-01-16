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


#include "eckit/geo/grid/regular/RegularGaussian.h"

#include "eckit/geo/iterator/Regular.h"
#include "eckit/geo/range/Gaussian.h"
#include "eckit/geo/range/Regular.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/utils/Translator.h"


namespace eckit::geo::grid::regular {


RegularGaussian::RegularGaussian(const Spec& spec) :
    RegularGaussian(spec.get_unsigned("N"), spec.get_unsigned("ni"), area::BoundingBox(spec)) {}


RegularGaussian::RegularGaussian(size_t N, const area::BoundingBox& bbox) :
    RegularGaussian(N, 4 * N, bbox) {}


RegularGaussian::RegularGaussian(size_t N, size_t ni, const area::BoundingBox& bbox) :
    Regular(bbox),
    x_(new range::Regular(ni, bbox.west, bbox.east)),
    y_(new range::Gaussian(N, bbox.north, bbox.south)) {
    ASSERT(x_->size() > 0);
    ASSERT(y_->size() > 0);
}


Grid::iterator RegularGaussian::cbegin() const {
    return iterator{new geo::iterator::Regular(*this, 0)};
}


Grid::iterator RegularGaussian::cend() const {
    return iterator{new geo::iterator::Regular(*this, size())};
}


Spec* RegularGaussian::spec(const std::string& name) {
    auto N = Translator<std::string, size_t>{}(name.substr(1));
    return new spec::Custom({{"type", "regular_gg"}, {"N", N}, {"ni", 4 * N}});
}


const std::vector<double>& RegularGaussian::longitudes() const {
    return x_->values();
}


const std::vector<double>& RegularGaussian::latitudes() const {
    return x_->values();
}


static const GridRegisterType<RegularGaussian> __grid_type("regular_gg");
static const GridRegisterName<RegularGaussian> __grid_pattern("[fF][1-9][0-9]*");


}  // namespace eckit::geo::grid::regular
