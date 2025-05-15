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
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/spec/Custom.h"


namespace eckit::geo::order {


static OrderRegisterType<Scan> ORDERING("scan");


const Order::value_type Scan::DEFAULT = "i+j-";


Scan::Scan(const value_type& order) : order_(order) {
    static const std::vector<Order::value_type> MODES{"i+j+",  "i+j-",  "i-j+",  "i-j-",
                                                      "i+j+-", "i+j-+", "i-j+-", "i-j-+"};

    if (std::count(MODES.begin(), MODES.end(), order_) != 1) {
        throw exception::OrderError("Scan invalid order: '" + order_ + "'", Here());
    }
}


Scan::Scan(const Spec& spec) : Scan(spec.get_string("order", DEFAULT)) {}


bool Scan::is_scan_i_positively(const value_type& o) {
    return o.find("i+") != value_type::npos;
}


bool Scan::is_scan_j_positively(const value_type& o) {
    return o.find("j+") != value_type::npos;
}


bool Scan::is_scan_alternating(const value_type& o) {
    return o.find("+-") != value_type::npos || o.find("-+") != value_type::npos;
}


const std::string& Scan::static_type() {
    static const std::string type{"scan"};
    return type;
}


Reordering Scan::reorder(const value_type& to) const {
    NOTIMP;
}


size_t Scan::size() const {
    NOTIMP;
}


const Order::value_type& Scan::order_default() {
    return DEFAULT;
}


void Scan::fill_spec(spec::Custom& custom) const {
    if (order_ != order_default()) {
        custom.set("type", type());
        custom.set("order", order_);
    }
}


}  // namespace eckit::geo::order
