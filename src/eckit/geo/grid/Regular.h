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

    [[nodiscard]] static std::pair<Range*, Range*> make_lonlat_ranges_from_spec(const Spec&);

    // -- Overridden methods

    iterator cbegin() const override;
    iterator cend() const override;

    size_t size() const final { return nx() * ny(); }

    const Range& x() const;
    const Range& y() const;

protected:
    // -- Constructors

    Regular(std::pair<Range*, Range*> xy, const area::BoundingBox&);

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
