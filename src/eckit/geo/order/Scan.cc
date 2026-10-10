// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


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


Scan::renumber_type Scan::reorder(const order_type& to, size_t ni, size_t nj) const {
    ASSERT(0 < ni && 0 < nj);

    renumber_type ren(ni * nj);
    if (to == order_) {
        std::iota(ren.begin(), ren.end(), 0);
        return ren;
    }

    const auto from_canonical = canonical(ni, nj);
    const auto to_canonical   = Scan{to}.canonical(ni, nj);

    renumber_type to_index(ni * nj);
    for (size_t k = 0; k < to_canonical.size(); ++k) {
        to_index[to_canonical[k]] = k;
    }

    for (size_t k = 0; k < from_canonical.size(); ++k) {
        ren[k] = to_index[from_canonical[k]];
    }

    return ren;
}


Scan::renumber_type Scan::canonical(size_t ni, size_t nj) const {
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
