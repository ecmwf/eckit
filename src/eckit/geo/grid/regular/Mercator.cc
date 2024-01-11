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


#include "eckit/geo/grid/regular/Mercator.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/iterator/Regular.h"
#include "eckit/geo/spec/MappedConfiguration.h"
#include "eckit/geo/util/regex.h"
#include "eckit/types/Fraction.h"
#include "eckit/utils/Translator.h"


namespace eckit::geo::grid::regular {


static const GridRegisterType<Mercator> __grid_type("mercator");


Mercator::Mercator(const Configuration& config) :
    Mercator(config.getUnsigned("ni"), config.getUnsigned("nj"), Increments{config}, area::BoundingBox{config}) {}


Mercator::Mercator(size_t ni, size_t nj, const Increments& inc, const area::BoundingBox& bbox) :
    Regular(bbox), inc_(inc), ni_(ni), nj_(nj) {
    ASSERT(ni_ > 0);
    ASSERT(nj_ > 0);
}


Grid::iterator Mercator::cbegin() const {
    return iterator{new geo::iterator::Regular(*this, 0)};
}


Grid::iterator Mercator::cend() const {
    return iterator{new geo::iterator::Regular(*this, size())};
}

const std::vector<double>& Mercator::longitudes() const {
    NOTIMP;
}


const std::vector<double>& Mercator::latitudes() const {
    NOTIMP;
}


}  // namespace eckit::geo::grid::regular
