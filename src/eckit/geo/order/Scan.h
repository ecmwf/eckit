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

#include <memory>

#include "eckit/geo/Order.h"
#include "eckit/geo/util.h"


namespace eckit::geo::order {


class Scan final : public Order {
public:

    // -- Constructors

    Scan(const value_type&, size_t nx, size_t ny);
    Scan(const value_type&, const pl_type&);

    explicit Scan(const value_type&);
    explicit Scan(const Spec&);

    // -- Methods

    const std::string& type() const override;

    const value_type& order() const override { return order_; }
    Reordering reorder(const value_type& to) const override;

    // -- Class methods

    static const Order::value_type& order_default();
    static Order::value_type make_order_from_spec(const Spec& spec);

    static bool is_scan_i_positively(const value_type&);
    static bool is_scan_j_positively(const value_type&);
    static bool is_scan_i_j(const value_type&);
    static bool is_scan_alternating(const value_type&);

private:

    // -- Types

    struct Implementation {
        Implementation()          = default;
        virtual ~Implementation() = default;

        Implementation(const Implementation&) = delete;
        Implementation(Implementation&&)      = delete;
        void operator=(const Implementation&) = delete;
        void operator=(Implementation&&)      = delete;

        virtual Reordering reorder(const value_type& from, const value_type& to) const = 0;
    };

    // -- Constructors

    Scan(const value_type&, Implementation*);

    // -- Members

    std::unique_ptr<Implementation> impl_;
    value_type order_;

    static const value_type DEFAULT;

    // -- Overriden methods

    void fill_spec(spec::Custom&) const override;
};


}  // namespace eckit::geo::order
