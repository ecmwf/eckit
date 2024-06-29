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


#include "eckit/geo/grid/Regular.h"

#include <algorithm>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/iterator/Regular.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/types/Fraction.h"


namespace eckit::geo::grid {


namespace {


area::BoundingBox make_bounding_box(const Range& lon, const Range& lat) {
    auto n = std::max(lat.a(), lat.b());
    auto w = std::min(lon.a(), lon.b());
    auto s = std::min(lat.a(), lat.b());
    auto e = std::max(lon.a(), lon.b());
    return {n, w, s, e};
}


}  // namespace


double Regular::dx() const {
    return x().increment();
}


double Regular::dy() const {
    return y().increment();
}


Grid::iterator Regular::cbegin() const {
    return iterator{new geo::iterator::Regular(*this, 0)};
}


Grid::iterator Regular::cend() const {
    return iterator{new geo::iterator::Regular(*this, size())};
}


const Range& Regular::x() const {
    ASSERT(x_ && x_->size() > 0);
    return *x_;
}


const Range& Regular::y() const {
    ASSERT(y_ && y_->size() > 0);
    return *y_;
}


Regular::Regular(Ranges xy, Projection* projection) :
    Grid(make_bounding_box(*xy.first, *xy.second), projection), x_(xy.first), y_(xy.second) {
    ASSERT(x_ && x_->size() > 0);
    ASSERT(y_ && y_->size() > 0);
}


void Regular::fill_spec(spec::Custom& custom) const {
    Grid::fill_spec(custom);
}


Regular::Ranges::Ranges(Range* x, Range* y) : pair{x, y} {
    ASSERT(first != nullptr && second != nullptr);
}


}  // namespace eckit::geo::grid
