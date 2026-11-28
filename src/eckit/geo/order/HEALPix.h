// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <optional>
#include <utility>

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

        // Row (ring, north to south) and position within the row of a nested index
        std::pair<size_t, size_t> nest_to_row(size_t) const;

    private:

        const int Nside_;  // up to 2^13
        const int Npix_;
        const int Ncap_;
        const int k_;
    };

    // -- Constructors

    explicit HEALPix(size_t Nside, const order_type& = order_default());
    explicit HEALPix(const Spec&);

    // -- Methods

    size_t Nside() const { return Nside_; }
    const order_type& order() const { return order_; }

    /// Between the ring and nested index of a point, of the nested order
    const Renumber& renumber() const;

    renumber_type reorder(const order_type& to) const;

    // -- Class members

    static const order_type RING;
    static const order_type NESTED;

    // -- Class methods

    static const order_type& order_default() { return RING; }

    // -- Deprecated (see deprecated.cc)

    [[deprecated("use HEALPix(Nside, order)")]] explicit HEALPix(const order_type&);
    [[deprecated("use HEALPix(Nside, order).reorder(to)")]] renumber_type reorder(const order_type& to,
                                                                                  size_t Nside) const;

private:

    // -- Members

    size_t Nside_;
    order_type order_;
    std::optional<Renumber> renumber_;  // of the nested order, checking Nside
};


}  // namespace eckit::geo::order
