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
#include <utility>

#include "eckit/geo/Grid.h"
#include "eckit/geo/Range.h"


namespace eckit::geo {
class Increments;
namespace iterator {
class Regular;
}
}  // namespace eckit::geo


namespace eckit::geo::grid {


class Regular : public Grid {
public:
    // -- Constructors

    explicit Regular(const Spec& spec) : Grid(spec) {}

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

protected:
    // -- Types

    struct Ranges : std::pair<Range*, Range*> {
        Ranges(Range*, Range*);
    };

    // -- Constructors

    explicit Regular(Ranges, Projection* = nullptr);

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;

private:
    // -- Members

    std::unique_ptr<Range> x_;
    std::unique_ptr<Range> y_;

    // -- Friends

    friend class geo::iterator::Regular;
};


}  // namespace eckit::geo::grid
