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

#include "eckit/geometry/Iterator.h"


namespace eckit::geometry::iterator {


class ListI final : public Iterator {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    explicit ListI(const Grid&, size_t index = 0);

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

    const std::vector<double>& latitudes_;
    const std::vector<double>& longitudes_;
    const size_t size_;
    size_t index_;

    // -- Methods
    // None

    // -- Overridden methods

    bool operator==(const Iterator&) const override;
    bool operator++() override;
    bool operator+=(diff_t) override;

    explicit operator bool() const override;
    Point operator*() const override;

    size_t index() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geometry::iterator
