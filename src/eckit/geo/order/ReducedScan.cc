// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/order/ReducedScan.h"

#include <cstddef>
#include <numeric>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/spec/Spec.h"


namespace eckit::geo::order {


ReducedScan::ReducedScan(const order_type& order) : Scan(order, mode_t{}) {
    if (!is_scan_i_then_j() || is_scan_alternating()) {
        throw exception::OrderError(
            "ReducedScan: invalid order '" + this->order() + "', choices are: i+j-, i+j+, i-j-, i-j+", Here());
    }
}


ReducedScan::ReducedScan(const Spec& spec) : ReducedScan(spec.get_string("order", order_default())) {}


ReducedScan::renumber_type ReducedScan::canonical(const std::vector<size_t>& nxacc) const {
    ASSERT(!nxacc.empty() && nxacc.front() == 0);
    const auto ny = nxacc.size() - 1;

    const auto i_positive = is_scan_i_positive();
    const auto j_positive = is_scan_j_positive();

    renumber_type index;
    index.reserve(nxacc.back());

    for (size_t r = 0; r < ny; ++r) {
        const auto j = j_positive ? ny - 1 - r : r;
        ASSERT(nxacc[j] <= nxacc[j + 1]);

        const auto n = nxacc[j + 1] - nxacc[j];
        for (size_t k = 0; k < n; ++k) {
            index.emplace_back(nxacc[j] + (i_positive ? k : n - 1 - k));
        }
    }

    return index;
}


ReducedScan::renumber_type ReducedScan::reorder(const order_type& to, const std::vector<size_t>& nxacc) const {
    if (to == order()) {
        ASSERT(!nxacc.empty());
        renumber_type ren(nxacc.back());
        std::iota(ren.begin(), ren.end(), 0);
        return ren;
    }

    const auto from_canonical = canonical(nxacc);
    const auto to_canonical   = ReducedScan{to}.canonical(nxacc);

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
