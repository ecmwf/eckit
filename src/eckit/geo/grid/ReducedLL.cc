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


#include "eckit/geo/grid/ReducedLL.h"

#include "eckit/geo/iterator/Reduced.h"
#include "eckit/geo/range/RegularLatitude.h"
#include "eckit/geo/range/RegularLongitude.h"
#include "eckit/geo/spec/Custom.h"


namespace eckit::geo::grid {


ReducedLL::ReducedLL(const Spec& spec) : ReducedLL(spec.get_long_vector("pl"), area::BoundingBox(spec)) {}


ReducedLL::ReducedLL(const pl_type& pl, const area::BoundingBox& bbox) :
    Reduced(bbox), pl_(pl), y_(new range::RegularLatitude(pl_.size(), bbox.north, bbox.south)) {
    ASSERT(y_);
}


Grid::iterator ReducedLL::cbegin() const {
    return iterator{new geo::iterator::Reduced(*this, 0)};
}


Grid::iterator ReducedLL::cend() const {
    return iterator{new geo::iterator::Reduced(*this, size())};
}


size_t ReducedLL::ni(size_t j) const {
    return pl_.at(j);
}


size_t ReducedLL::nj() const {
    return pl_.size();
}


const std::vector<double>& ReducedLL::latitudes() const {
    return y_->values();
}


std::vector<double> ReducedLL::longitudes(size_t j) const {
    auto Ni = ni(j);
    if (!x_ || x_->size() != Ni) {
        auto bbox                               = boundingBox();
        const_cast<std::unique_ptr<Range>&>(x_) = std::make_unique<range::RegularLongitude>(Ni, bbox.west, bbox.east);
    }

    return x_->values();
}


void ReducedLL::fill_spec(spec::Custom& custom) const {
    Reduced::fill_spec(custom);

    custom.set("type", "reduced_ll");
    custom.set("pl", pl_);
}


}  // namespace eckit::geo::grid
