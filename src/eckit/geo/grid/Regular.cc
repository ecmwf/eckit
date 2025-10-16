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


const Regular::order_type& Regular::order() const {
    return order_.order();
}


Grid::renumber_type Regular::reorder(const order_type& to) const {
    return order_.reorder(to, nx(), ny());
}


const Range& Regular::x() const {
    ASSERT(x_ && x_->size() > 0);
    return *x_;
}


const Range& Regular::y() const {
    ASSERT(y_ && y_->size() > 0);
    return *y_;
}


Regular::Regular(const Spec& spec) : Grid(spec), order_(spec) {
    ASSERT(x_ && x_->size() > 0);
    ASSERT(y_ && y_->size() > 0);
}


Regular::Regular(Range* x, Range* y, const Projection* projection) : x_(x), y_(y) {
    ASSERT(x_ && x_->size() > 0);
    ASSERT(y_ && y_->size() > 0);
}


void Regular::fill_spec(spec::Custom& custom) const {
    Grid::fill_spec(custom);
}

void Regular::order(const order_type& to) {
    order_.order(to);
}


}  // namespace eckit::geo::grid
