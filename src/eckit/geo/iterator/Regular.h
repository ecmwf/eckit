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

#include "eckit/geo/Iterator.h"


namespace eckit::geo::grid {
class Regular;
}


namespace eckit::geo::iterator {


class Regular final : public Iterator {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    explicit Regular(const Grid&, size_t index = 0);

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    const grid::Regular& grid_;
    const std::vector<double>& longitudes_;
    const std::vector<double>& latitudes_;
    size_t i_;
    size_t j_;
    size_t index_;
    const size_t ni_;
    const size_t nj_;
    const size_t index_size_;

    // -- Methods
    // None

    // -- Overridden methods

    bool operator==(const Iterator&) const override;
    bool operator++() override;
    bool operator+=(diff_t) override;
    explicit operator bool() const override;
    Point operator*() const override;

    size_t index() const override { return index_; }

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geo::iterator