// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <vector>

#include "eckit/geo/order/Scan.h"


namespace eckit::geo::order {


/**
 * Scanning mode of the points of a reduced grid, of rows (j) of points (i): rows from north to south (j-) or from south
 * to north (j+), each from west to east (i+) or from east to west (i-). As rows have different numbers of points,
 * points are consecutive along rows, and rows don't alternate direction: i+j- (default), i+j+, i-j-, i-j+
 */
class ReducedScan : public Scan {
public:

    // -- Constructors

    explicit ReducedScan(const order_type& = order_default());
    explicit ReducedScan(const Spec&);

    // -- Methods

    /// Renumber in the default order of each point in this order, of rows (in the default order) by the points before
    /// each row, then the number of points
    renumber_type canonical(const std::vector<size_t>& nxacc) const;

    /// Renumbering of the points, in this order, to another order (of rows as canonical)
    renumber_type reorder(const order_type& to, const std::vector<size_t>& nxacc) const;
};


}  // namespace eckit::geo::order
