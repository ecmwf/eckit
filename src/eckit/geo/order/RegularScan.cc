// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/order/RegularScan.h"

#include <numeric>

#include "eckit/geo/Exceptions.h"
#include "eckit/spec/Spec.h"


namespace eckit::geo::order {


RegularScan::RegularScan(const order_type& order) : Scan(order) {}


RegularScan::RegularScan(const Spec& spec) : RegularScan(spec.get_string("order", order_default())) {}


RegularScan::renumber_type RegularScan::reorder(const order_type& to, size_t ni, size_t nj) const {
    ASSERT(0 < ni && 0 < nj);

    renumber_type ren(ni * nj);
    if (to == order()) {
        std::iota(ren.begin(), ren.end(), 0);
        return ren;
    }

    const auto from_canonical = canonical(ni, nj);
    const auto to_canonical   = RegularScan{to}.canonical(ni, nj);

    renumber_type to_index(ni * nj);
    for (size_t k = 0; k < to_canonical.size(); ++k) {
        to_index[to_canonical[k]] = k;
    }

    for (size_t k = 0; k < from_canonical.size(); ++k) {
        ren[k] = to_index[from_canonical[k]];
    }

    return ren;
}


RegularScan::renumber_type RegularScan::canonical(size_t ni, size_t nj) const {
    const auto i_positive  = is_scan_i_positive();
    const auto j_positive  = is_scan_j_positive();
    const auto alternating = is_scan_alternating();

    renumber_type index;
    index.reserve(ni * nj);

    if (is_scan_i_then_j()) {
        for (size_t j = 0; j < nj; ++j) {
            const auto row = j_positive ? nj - 1 - j : j;
            const auto pos = i_positive != (alternating && j % 2 == 1);
            for (size_t i = 0; i < ni; ++i) {
                index.emplace_back(row * ni + (pos ? i : ni - 1 - i));
            }
        }
    }
    else {
        for (size_t i = 0; i < ni; ++i) {
            const auto col = i_positive ? i : ni - 1 - i;
            const auto pos = j_positive != (alternating && i % 2 == 1);
            for (size_t j = 0; j < nj; ++j) {
                index.emplace_back((pos ? nj - 1 - j : j) * ni + col);
            }
        }
    }

    return index;
}


}  // namespace eckit::geo::order
