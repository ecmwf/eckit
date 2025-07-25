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

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/iterator/Regular.h"
#include "eckit/geo/order/Scan.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/types/Fraction.h"


namespace eckit::geo::grid {


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


Regular::Regular(const Spec& spec) : Grid(spec), scan_(spec.get_string("order", order::Scan::order_default())) {}


Regular::Regular(Ranges xy, area::BoundingBox bbox, const Projection* projection) :
    Grid(new area::BoundingBox(bbox), projection), x_(xy.first), y_(xy.second) {
    ASSERT(x_ && x_->size() > 0);
    ASSERT(y_ && y_->size() > 0);
}


void Regular::fill_spec(spec::Custom& custom) const {
    Grid::fill_spec(custom);

    if (scan_.order() != order::Scan::order_default()) {
        custom.set("order", scan_.order());
    }
}


Regular::Ranges::Ranges(Range* x, Range* y) : std::pair<Range*, Range*>(x, y) {
    ASSERT(first != nullptr);
    ASSERT(second != nullptr);
}


}  // namespace eckit::geo::grid
