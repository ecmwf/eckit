/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to anj jurisdiction.
 */


#include "eckit/geo/order/Scan.h"

#include <algorithm>
#include <numeric>
#include <vector>

#include "eckit/geo/Exceptions.h"


namespace eckit::geo::order {


static const Scan::order_type IPOS_JPOS{"i+j+"};
static const Scan::order_type IPOS_JNEG{"i+j-"};
static const Scan::order_type INEG_JPOS{"i-j+"};
static const Scan::order_type INEG_JNEG{"i-j-"};
static const Scan::order_type INEGPOS_JPOS{"i-+j+"};
static const Scan::order_type INEGPOS_JNEG{"i-+j-"};
static const Scan::order_type IPOSNEG_JPOS{"i+-j+"};
static const Scan::order_type IPOSNEG_JNEG{"i+-j-"};
static const Scan::order_type JPOS_IPOS{"j+i+"};
static const Scan::order_type JPOS_INEG{"j+i-"};
static const Scan::order_type JNEG_IPOS{"j-i+"};
static const Scan::order_type JNEG_INEG{"j-i-"};
static const Scan::order_type JNEGPOS_IPOS{"j-+i+"};
static const Scan::order_type JNEGPOS_INEG{"j-+i-"};
static const Scan::order_type JPOSNEG_IPOS{"j+-i+"};
static const Scan::order_type JPOSNEG_INEG{"j+-i-"};


static const std::vector<Scan::order_type> MODES{
    IPOS_JPOS, IPOS_JNEG, INEG_JPOS, INEG_JNEG, INEGPOS_JPOS, INEGPOS_JNEG, IPOSNEG_JPOS, IPOSNEG_JNEG,
    JPOS_IPOS, JPOS_INEG, JNEG_IPOS, JNEG_INEG, JNEGPOS_IPOS, JNEGPOS_INEG, JPOSNEG_IPOS, JPOSNEG_INEG,
};


Scan::Scan(const order_type& order) : order_(order) {
    if (std::count(MODES.begin(), MODES.end(), order_) != 1) {
        throw exception::OrderError("Scan invalid order: '" + order_ + "'", Here());
    }
}


Scan::Scan(const Spec& spec) : Scan(spec.get_string("order", order_default())) {}


bool Scan::is_scan_i_positive() const {
    return order_.find("i+") != order_type::npos;
}


bool Scan::is_scan_j_positive() const {
    return order_.find("j+") != order_type::npos;
}


bool Scan::is_scan_alternating() const {
    return order_.find("+-") != order_type::npos || order_.find("-+") != order_type::npos;
}


const Scan::order_type& Scan::order_default() {
    return IPOS_JNEG;
}


Scan::renumber_type Scan::reorder(const order_type& to, size_t ni, size_t nj) const {
    ASSERT(0 < ni && 0 < nj);

    if (to == order_) {
        // no reordering
        renumber_type ren(ni * nj);
        std::iota(ren.begin(), ren.end(), 0);
        return ren;
    }

    // TODO regular grid reordering
    NOTIMP;
}


Scan::renumber_type Scan::reorder(const order_type& to, const pl_type& pl) const {
    ASSERT(2 <= pl.size());
    ASSERT(std::all_of(pl.begin(), pl.end(), [](auto n) { return 2 <= n; }));

    if (to == order_) {
        // no reordering
        auto size = static_cast<size_t>(std::accumulate(pl.begin(), pl.end(), static_cast<pl_type::value_type>(0)));

        renumber_type ren(size);
        std::iota(ren.begin(), ren.end(), 0);
        return ren;
    }

    // TODO reduced grid reordering
    NOTIMP;
}


}  // namespace eckit::geo::order
