// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/grid/Regular.h"

#include "eckit/geo/iterator/Regular.h"
#include "eckit/spec/Custom.h"


namespace eckit::geo::grid {


Grid::iterator Regular::cbegin() const {
    return iterator{new geo::iterator::Regular(*this, 0)};
}


Grid::iterator Regular::cend() const {
    return iterator{new geo::iterator::Regular(*this, size())};
}


const Regular::order_type& Regular::order() const {
    return scan_.order();
}


Grid::renumber_type Regular::reorder(const order_type& to) const {
    return scan_.reorder(to, nx(), ny());
}


const order::Scan& Regular::scan_default() {
    static const order::Scan SCAN("i+j-");
    return SCAN;
}


Regular::Regular(order::Scan s, const Projection* proj) : Grid(nullptr, proj), scan_(s) {}


void Regular::fill_spec(spec::Custom& custom) const {
    Grid::fill_spec(custom);

    if (scan_.order() != order::Scan::order_default()) {
        custom.set("order", scan_.order());
    }
}

void Regular::scan(const order_type& to) {
    scan_.order(to);
}

void Regular::scan(order::Scan s) {
    scan_ = s;
}


}  // namespace eckit::geo::grid
