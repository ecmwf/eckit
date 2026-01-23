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

    virtual double dx() const = 0;
    virtual double dy() const = 0;

    virtual size_t nx() const = 0;
    virtual size_t ny() const = 0;

    virtual const Range& x() const = 0;
    virtual const Range& y() const = 0;

    // -- Overridden methods

    iterator cbegin() const final;
    iterator cend() const final;

    size_t size() const final { return nx() * ny(); }
    std::vector<size_t> shape() const final { return {ny(), nx()}; }

    const order_type& order() const final;
    renumber_type reorder(const order_type& to) const final;

protected:

    // -- Constructors

    explicit Regular(const Spec&);
    explicit Regular(const Projection* = nullptr);

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;

    // -- Methods

    void order(const order_type& to);

private:

    // -- Members

    order::Scan order_;

    // -- Friends

    friend class geo::iterator::Regular;
};


}  // namespace eckit::geo::grid
