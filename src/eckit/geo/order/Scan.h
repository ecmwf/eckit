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
#include "eckit/geo/util.h"


namespace eckit::geo::order {


class Scan final : public Order {
public:

    // -- Constructors

    explicit Scan(const value_type& = order_default());
    explicit Scan(const Spec&);

    // -- Overriden methods

    const std::string& type() const override { return static_type(); }
    const value_type& order() const override { return order_; }

    Reordering reorder(const value_type& to) const override;
    size_t size() const override;

    // -- Methods

    bool is_scan_i_positively(const value_type&);
    bool is_scan_j_positively(const value_type&);
    bool is_scan_alternating(const value_type&);

    // -- Class methods

    static const Order::value_type& order_default();
    static Order::value_type order_from_arguments(bool i_pos, bool j_pos, bool ij, bool alt);

private:

    // -- Members

    value_type order_;

    // -- Overriden methods

    void fill_spec(spec::Custom&) const override;

    // -- Class members

    static const value_type DEFAULT;

    // -- Class methods

    static const std::string& static_type();
};


}  // namespace eckit::geo::order
