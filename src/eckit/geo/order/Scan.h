// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include "eckit/geo/Grid.h"


namespace eckit::spec {
class Spec;
}


namespace eckit::geo::order {


/**
 * Scanning mode (order) of grid points: along i (x, longitude) then j (y, latitude) or j then
 * i, each positive or negative, and alternating direction or not (e.g. i+j-, the default: rows north to south, each
 * west to east). Grids of the supported modes use RegularScan (all modes) or ReducedScan (rows of points)
 */
class Scan {
public:

    // -- Types

    using order_type    = Grid::order_type;
    using renumber_type = Grid::renumber_type;
    using Spec          = spec::Spec;

    // -- Methods

    const order_type& order() const { return order_; }

    bool is_default() const { return order_ == order_default(); }
    bool is_scan_i_then_j() const;
    bool is_scan_i_positive() const;
    bool is_scan_j_positive() const;
    bool is_scan_alternating() const;

    // -- Class methods

    static const order_type& order_default();

protected:

    // -- Constructors

    /// Scanning mode, of any of the scanning modes (empty for the default)
    explicit Scan(const order_type&);

    Scan(const Scan&) = default;
    Scan(Scan&&)      = default;

    // -- Destructor

    ~Scan() = default;

    // -- Operators

    Scan& operator=(const Scan&) = default;
    Scan& operator=(Scan&&)      = default;

private:

    // -- Members

    order_type order_;
};


}  // namespace eckit::geo::order
