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

#include "eckit/geo/Grid.h"
#include "eckit/geo/Range.h"
#include "eckit/geo/order/Scan.h"


namespace eckit::geo::iterator {
class Regular;
}  // namespace eckit::geo::iterator


namespace eckit::geo::grid {


class Regular : public Grid {
public:

    // -- Methods

    virtual double dx() const;
    virtual double dy() const;

    virtual size_t nx() const { return x_->size(); }
    virtual size_t ny() const { return y_->size(); }

    const Range& x() const;
    const Range& y() const;

    // -- Overridden methods

    iterator cbegin() const override;
    iterator cend() const override;

    size_t size() const final { return nx() * ny(); }
    std::vector<size_t> shape() const override { return {ny(), nx()}; }

    const order_type& order() const override;
    renumber_type reorder(const order_type& to) const override;

protected:

    // -- Constructors

    explicit Regular(const Spec&);
    explicit Regular(Range* x, Range* y, const Projection* = nullptr);

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;

    // -- Methods

    void order(const order_type& to);

private:

    // -- Members

    const std::unique_ptr<Range> x_;
    const std::unique_ptr<Range> y_;
    order::Scan order_;

    // -- Friends

    friend class geo::iterator::Regular;
};


}  // namespace eckit::geo::grid
