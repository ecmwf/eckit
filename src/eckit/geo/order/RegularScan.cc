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


#include "eckit/geo/order/RegularScan.h"

#include "eckit/geo/Exceptions.h"


namespace eckit::geo::order {


static ReorderRegisterType<RegularScan> ORDERING("regular-scan");


const std::string& RegularScan::type() const {
    static const std::string type{"regular-scan"};
    return type;
}


const Order::value_type& RegularScan::order_default() const {
    return scan_i_positively_j_negatively;
}


Reordering RegularScan::reorder(const value_type& to) const {
    if (order_ == to) {
        return no_reorder(nx_ * ny_);
    }

    ASSERT(not is_scan_alternating_direction(order_));
    ASSERT(not is_scan_alternating_direction(to));

    Reordering ren(nx_ * ny_);

    if (order_ == scan_i_positively_j_positively) {
        size_t count = 0;
        for (size_t j = ny_; j > 0; --j) {
            for (size_t i = 0; i < nx_; ++i) {
                ren[count++] = (j - 1) * nx_ + i;
            }
        }

        return ren;
    }

    if (order_ == scan_i_negatively_j_negatively) {
        size_t count = 0;
        for (size_t j = 0; j < ny_; ++j) {
            for (size_t i = nx_; i > 0; --i) {
                ren[count++] = j * nx_ + (i - 1);
            }
        }

        return ren;
    }

    if (order_ == scan_i_negatively_j_positively) {
        size_t count = 0;
        for (size_t j = ny_; j > 0; --j) {
            for (size_t i = nx_; i > 0; --i) {
                ren[count++] = (j - 1) * nx_ + (i - 1);
            }
        }

        return ren;
    }

    throw exception::ReorderError("RegularScan::reorder(" + order_ + ", " + to + ")", Here());
}


}  // namespace eckit::geo::order
