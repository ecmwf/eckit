/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#pragma once

#include "eckit/geo/Grid.h"


namespace eckit::geo::order {


class HEALPix {
public:

    // -- Types

    using order_type    = Grid::order_type;
    using renumber_type = Grid::renumber_type;

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
