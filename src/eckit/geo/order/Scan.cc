// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/order/Scan.h"

#include <algorithm>
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


Scan::Scan(const order_type& order) : order_(order.empty() ? order_default() : order) {
    if (std::count(MODES.begin(), MODES.end(), order_) != 1) {
        throw exception::OrderError("Scan invalid order: '" + order_ + "'", Here());
    }
}


bool Scan::is_scan_i_then_j() const {
    ASSERT(!order_.empty());
    return order_.front() == 'i';
}


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


}  // namespace eckit::geo::order
