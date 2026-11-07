// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/order/ReducedScan.h"

#include <cstddef>
#include <numeric>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/spec/Spec.h"


namespace eckit::geo::order {


ReducedScan::ReducedScan(const order_type& order) : Scan(order) {
    if (!is_scan_i_then_j() || is_scan_alternating()) {
        throw exception::OrderError(
            "ReducedScan: invalid order '" + this->order() + "', choices are: i+j-, i+j+, i-j-, i-j+", Here());
    }
}


ReducedScan::ReducedScan(const Spec& spec) : ReducedScan(spec.get_string("order", order_default())) {}


ReducedScan::renumber_type ReducedScan::canonical(const pl_type& pl) const {
    // points before each row (rows in the default order)
    std::vector<size_t> start(pl.size() + 1, 0);
    for (size_t j = 0; j < pl.size(); ++j) {
        ASSERT(0 <= pl[j]);
        start[j + 1] = start[j] + static_cast<size_t>(pl[j]);
    }

    const auto i_positive = is_scan_i_positive();
    const auto j_positive = is_scan_j_positive();

    renumber_type index;
    index.reserve(start.back());

    for (size_t r = 0; r < pl.size(); ++r) {
        const auto j = j_positive ? pl.size() - 1 - r : r;
        const auto n = start[j + 1] - start[j];
        for (size_t k = 0; k < n; ++k) {
            index.emplace_back(start[j] + (i_positive ? k : n - 1 - k));
        }
    }

    return index;
}


ReducedScan::renumber_type ReducedScan::reorder(const order_type& to, const pl_type& pl) const {
    if (to == order()) {
        renumber_type ren(static_cast<size_t>(std::accumulate(pl.begin(), pl.end(), pl_type::value_type{0})));
        std::iota(ren.begin(), ren.end(), 0);
        return ren;
    }

    const auto from_canonical = canonical(pl);
    const auto to_canonical   = ReducedScan{to}.canonical(pl);

    renumber_type to_index(to_canonical.size());
    for (size_t k = 0; k < to_canonical.size(); ++k) {
        to_index[to_canonical[k]] = k;
    }

    renumber_type ren(from_canonical.size());
    for (size_t k = 0; k < from_canonical.size(); ++k) {
        ren[k] = to_index[from_canonical[k]];
    }

    return ren;
}


}  // namespace eckit::geo::order
