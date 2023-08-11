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

#include "eckit/geometry/Point.h"


namespace eckit::geometry {


class Iterator {
public:
    // -- Types
    // None

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

    virtual bool operator!=(const Iterator&) = 0;
    virtual bool operator++()                = 0;
    virtual explicit operator bool()         = 0;
    virtual const Point& operator*() const   = 0;

    // -- Methods

    virtual size_t size() const  = 0;
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


}  // namespace eckit::geometry
