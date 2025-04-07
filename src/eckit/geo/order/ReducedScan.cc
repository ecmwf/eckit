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


#include "eckit/geo/order/ReducedScan.h"

#include <numeric>

#include "eckit/geo/Exceptions.h"


namespace eckit::geo::order {


static ReorderRegisterType<ReducedScan> ORDERING("reduced-scan");


const std::string& ReducedScan::type() const {
    static const std::string type{"reduced-scan"};
    return type;
}


const Order::value_type& ReducedScan::order_default() const {
    return scan_i_positively_j_negatively;
}


Reordering ReducedScan::reorder(const value_type& to) const {
    if (order_ == to) {
        const auto size = std::accumulate(pl_.begin(), pl_.end(), static_cast<size_t>(0));
        return no_reorder(size);
    }

    // FIXME implement this

    throw exception::ReorderError("ReducedScan::reorder(" + order_ + ", " + to + ")", Here());
}

}  // namespace eckit::geo::order
