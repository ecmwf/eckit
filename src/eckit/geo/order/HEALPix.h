// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>

#include "eckit/geo/Grid.h"
#include "eckit/spec/Spec.h"


namespace eckit::geo::order {


class HEALPix {
public:

    // -- Types

    using order_type    = Grid::order_type;
    using renumber_type = Grid::renumber_type;
    using Spec          = spec::Spec;

    /// Between the ring and nested index of a point; Nside must be a power of 2
    class Renumber {
    public:

        explicit Renumber(size_t Nside);

        size_t ring_to_nest(size_t index) const;
        size_t nest_to_ring(size_t index) const;

    private:

        const int Nside_;  // up to 2^13
        const int Npix_;
        const int Ncap_;
        const int k_;
    };

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
