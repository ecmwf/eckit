// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "eckit/geo/Grid.h"


namespace eckit::geo::order {


class HEALPix {
public:

    // -- Types

    using order_type    = Grid::order_type;
    using renumber_type = Grid::renumber_type;
    using Spec          = spec::Spec;

    // -- Constructors

    explicit HEALPix(const order_type& = order_default());
    explicit HEALPix(const Spec&);

    // -- Methods

    const order_type& order() const { return order_; }
    renumber_type reorder(const order_type& to, size_t Nside) const;

    // -- Class members

    static const order_type RING;
    static const order_type NESTED;

    // -- Class methods

    static const order_type& order_default() { return RING; }

private:

    // -- Members

    order_type order_;
};


}  // namespace eckit::geo::order
