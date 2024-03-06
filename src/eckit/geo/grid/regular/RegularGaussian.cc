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

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/iterator/Regular.h"
#include "eckit/geo/range/GaussianLatitude.h"
#include "eckit/geo/range/RegularLongitude.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/utils/Translator.h"


namespace eckit::geo::grid::regular {


RegularGaussian::RegularGaussian(const Spec& spec) :
    RegularGaussian(spec.get_unsigned("N"), area::BoundingBox(spec)) {}


RegularGaussian::RegularGaussian(size_t N, const area::BoundingBox& bbox) :
    Regular(bbox),
    N_(N),
    x_(range::RegularLongitude(4 * N, 0., 360.).crop(bbox.west, bbox.east)),
    y_(range::GaussianLatitude(N, false).crop(bbox.north, bbox.south)) {
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
    return new spec::Custom({{"type", "regular_gg"}, {"N", N}});
}


const std::vector<double>& RegularGaussian::longitudes() const {
    return x_->values();
}


const std::vector<double>& RegularGaussian::latitudes() const {
    return y_->values();
}


void RegularGaussian::spec(spec::Custom& custom) const {
    ASSERT(!custom.has("grid"));
    custom.set("grid", "F" + std::to_string(N_));
}


Grid* RegularGaussian::grid_crop(const area::BoundingBox& crop) const {
    if (auto bbox(boundingBox()); crop.intersects(bbox)) {
        return new RegularGaussian(N_, bbox);
    }

    throw eckit::Exception("RegularGaussian: cannot crop grid (empty intersection)");
}


static const GridRegisterType<RegularGaussian> __grid_type("regular_gg");
static const GridRegisterName<RegularGaussian> __grid_pattern("[fF][1-9][0-9]*");


}  // namespace eckit::geo::grid::regular
