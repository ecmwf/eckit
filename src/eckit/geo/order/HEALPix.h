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

#include "eckit/geo/Order.h"


namespace eckit::geo::order {


class HEALPix final : public Order {
public:

    // -- Constructors

    explicit HEALPix(const value_type& order, int Nside = 1);
    explicit HEALPix(const Spec&);

    // -- Methods

    int size() const { return 12 * Nside_ * Nside_; }
    int nside() const { return Nside_; }

    int ring_to_nest(int r) const;
    int nest_to_ring(int n) const;

    // -- Overriden methods

    const std::string& type() const override;

    const value_type& order() const override { return order_; }
    Reordering reorder(const value_type& to) const override;

    // -- Class members

    static const value_type ring;
    static const value_type nested;

    // -- Class methods

    static const Order::value_type& order_default() { return ring; }

private:

    // -- Members

    value_type order_;

    const int Nside_;  // up to 2^13
    const int Npix_;
    const int Ncap_;
    const int k_;

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;
};


}  // namespace eckit::geo::order
