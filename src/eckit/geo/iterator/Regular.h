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

#include <vector>

#include "eckit/geo/Iterator.h"


namespace eckit::geo {
class Range;
namespace grid {
class Regular;
}
}  // namespace eckit::geo


namespace eckit::geo::iterator {


class Regular final : public Iterator {
public:
    // -- Constructors

    explicit Regular(const grid::Regular&, size_t index = 0);

private:
    // -- Members

    const grid::Regular& grid_;
    const std::vector<double>& x_;
    const std::vector<double>& y_;
    size_t i_;
    size_t j_;
    size_t index_;
    const size_t nx_;
    const size_t ny_;
    const size_t size_;

    // -- Overridden methods

    bool operator==(const Iterator&) const override;
    bool operator++() override;
    bool operator+=(difference_type) override;
    explicit operator bool() const override;
    Point operator*() const override;

    size_t index() const override { return index_; }

    void fill_spec(spec::Custom&) const override;
};


}  // namespace eckit::geo::iterator
