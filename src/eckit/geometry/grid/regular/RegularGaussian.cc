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


#include "eckit/geometry/grid/regular/RegularGaussian.h"

#include "eckit/config/MappedConfiguration.h"
#include "eckit/geometry/iterator/Regular.h"
#include "eckit/geometry/range/Gaussian.h"
#include "eckit/geometry/range/LocalRegular.h"
#include "eckit/utils/Translator.h"


namespace eckit::geometry::grid::regular {
// LocalRegular(size_t n, double a, double b, bool endpoint, double precision = 0.);


RegularGaussian::RegularGaussian(const Configuration& config) :
    Regular(config),
    ni_(config.getUnsigned("ni", config.getUnsigned("ni"))),
    x_(new range::LocalRegular(ni_, config.getDouble("west", 0.), config.getDouble("east", 360.), true /*FIXME*/)),
    y_(new range::Gaussian(config.getUnsigned("N"),
                           config.getDouble("north", 90.),
                           config.getDouble("south", -90.))) {
    ASSERT(ni_ > 0);
}


RegularGaussian::RegularGaussian(size_t N, const area::BoundingBox& bbox) :
    Regular(bbox),
    ni_(4 * N),
    x_(new range::LocalRegular(ni_, bbox.west(), bbox.east(), true /*FIXME*/)),
    y_(new range::Gaussian(N, bbox.west(), bbox.east())) {
}


Grid::iterator RegularGaussian::cbegin() const {
    return iterator{new geometry::iterator::Regular(*this, 0)};
}


Grid::iterator RegularGaussian::cend() const {
    return iterator{new geometry::iterator::Regular(*this, size())};
}


Configuration* RegularGaussian::config(const std::string& name) {
    auto N = Translator<std::string, size_t>{}(name.substr(1));
    return new MappedConfiguration({{"type", "regular_gg"},
                                    {"N", N},
                                    {"Ni", 2 * N},
                                    {"Nj", 4 * N}});
}


const std::vector<double>& RegularGaussian::longitudes() const {
    return x_->values();
}


const std::vector<double>& RegularGaussian::latitudes() const {
    return x_->values();
}


static const GridRegisterType<RegularGaussian> __grid_type("regular_gg");
static const GridRegisterName<RegularGaussian> __grid_pattern("[fF][1-9][0-9]*");


}  // namespace eckit::geometry::grid::regular
