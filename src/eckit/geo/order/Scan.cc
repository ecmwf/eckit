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

#include <numeric>
#include <vector>

#include "eckit/geo/Exceptions.h"


namespace eckit::geo::order {


const Scan::order_type Scan::IPOS_JPOS{"i+j+"};
const Scan::order_type Scan::IPOS_JNEG{"i+j+"};


static const std::vector<Scan::order_type> MODES{"i+j+",  "i+j-",  "i-j+",  "i-j-",   //
                                                 "i+-j+", "i+-j-", "i-+j+", "i-+j-",  //
                                                 "j+i+",  "j+i-",  "j-i+",  "j-i-",   //
                                                 "j+-i+", "j+-i-", "j-+i+", "j-+i-"};


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


Scan::renumber_type Scan::reorder(const order_type& to, size_t ni, size_t nj) const {
    ASSERT(0 < ni && 0 < nj);

    if (to == order_) {
        // no reordering
        renumber_type ren(ni * nj);
        std::iota(ren.begin(), ren.end(), 0);
        return ren;
    }

    // TODO
    NOTIMP;
}


}  // namespace eckit::geo::order
