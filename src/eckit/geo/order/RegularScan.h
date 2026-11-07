// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>

#include "eckit/geo/order/Scan.h"


namespace eckit::geo::order {


/// Scanning mode of the points of a regular grid (ni by nj): any of the scanning modes (see Scan)
class RegularScan : public Scan {
public:

    // -- Constructors

    explicit RegularScan(const order_type& = order_default());
    explicit RegularScan(const Spec&);

    // -- Methods

    using Scan::order;
    void order(const order_type& to) { *this = RegularScan{to}; }

    /// Renumbering of the points, in this order, to another order
    renumber_type reorder(const order_type& to, size_t ni, size_t nj) const;

private:

    // -- Methods

    /// Renumber in the default order (i+j-) of each point, in this order
    renumber_type canonical(size_t ni, size_t nj) const;
};


}  // namespace eckit::geo::order
