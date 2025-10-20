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
#include "eckit/geo/util.h"


namespace eckit::geo::order {


class Scan {
public:

    // -- Types

    using order_type    = Grid::order_type;
    using renumber_type = Grid::renumber_type;

    // -- Constructors

    explicit Scan(const order_type& = order_default());
    explicit Scan(const Spec&);

    Scan(const Scan&) = default;
    Scan(Scan&&)      = default;

    // -- Operators

    Scan& operator=(const Scan&) = default;
    Scan& operator=(Scan&&)      = default;

    // -- Methods

    void order(const order_type& to) { operator=(Scan{to}); }

    const order_type& order() const { return order_; }
    renumber_type reorder(const order_type& to, size_t ni, size_t nj) const;
    renumber_type reorder(const order_type& to, const pl_type&) const;

    bool is_scan_i_positive() const;
    bool is_scan_j_positive() const;
    bool is_scan_alternating() const;

    // -- Class methods

    static const order_type& order_default();

private:

    // -- Members

    order_type order_;
};


}  // namespace eckit::geo::order
