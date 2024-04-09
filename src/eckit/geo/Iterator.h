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

#include <cstddef>

#include "eckit/geo/Point.h"


namespace eckit::geo {
class Grid;
}


namespace eckit::geo {


class Iterator {
public:
    // -- Types

    using difference_type = std::ptrdiff_t;

    // -- Exceptions
    // None

    // -- Constructors

    Iterator(const Iterator&) = delete;
    Iterator(Iterator&&)      = delete;

    // -- Destructor

    virtual ~Iterator() = default;

    // -- Convertors
    // None

    // -- Operators

    void operator=(const Iterator&) = delete;
    void operator=(Iterator&&)      = delete;

    virtual bool operator==(const Iterator&) const = 0;
    bool operator!=(const Iterator& other) const { return !operator==(other); }

    virtual bool operator++()                = 0;
    virtual bool operator+=(difference_type) = 0;

    virtual bool operator--() { return operator-=(1); }
    virtual bool operator-=(difference_type diff) { return operator+=(-diff); }

    virtual explicit operator bool() const = 0;
    virtual Point operator*() const        = 0;

    // -- Methods

    virtual size_t index() const = 0;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
    // -- Constructors

    Iterator() = default;

    // -- Members
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
    // None

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geo