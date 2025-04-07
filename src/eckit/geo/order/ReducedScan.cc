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


Reordering ReducedScan::reorder(const value_type& from, const value_type& to) const {
    const auto _from = from.empty() ? "scan_i_positively_j_negatively_ij_i_single_direction" : from;

    if (_from == to) {
        const auto size = std::accumulate(pl_.begin(), pl_.end(), static_cast<size_t>(0));
        return no_reorder(size);
    }

    // FIXME implement this

    throw exception::ReorderError("ReducedScan::reorder(" + from + ", " + to + ")", Here());
}

void ReducedScan::fill_spec(spec::Custom&) const {}


}  // namespace eckit::geo::order
