// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include "eckit/geo/Grid.h"
#include "eckit/geo/order/Scan.h"


namespace eckit::geo::iterator {
class Regular;
}


namespace eckit::geo::grid {


class Regular : public Grid {
public:

    // -- Overridden methods

    iterator cbegin() const final;
    iterator cend() const final;

    size_t size() const final { return nx() * ny(); }
    std::vector<size_t> shape() const final { return {ny(), nx()}; }

    const order_type& order() const final;
    renumber_type reorder(const order_type& to) const final;

    // -- Class methods

    static const order::Scan& scan_default();

protected:

    // -- Constructors

    explicit Regular(const Spec&);
    explicit Regular(order::Scan scan = scan_default(), Projection* = nullptr);

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;

    // -- Methods

    void scan(const order_type& to);
    void scan(order::Scan);
    const order::Scan& scan() const { return scan_; }

private:

    // -- Members

    order::Scan scan_;

    // -- Friends

    friend class geo::iterator::Regular;
};


}  // namespace eckit::geo::grid
