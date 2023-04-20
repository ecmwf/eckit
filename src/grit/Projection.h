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

#include "grit/types.h"


namespace grit {


class Projection {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    Projection() noexcept = default;

    Projection(const Projection&) = default;
    Projection(Projection&&)      = default;

    // -- Destructor

    virtual ~Projection() = default;

    // -- Convertors
    // None

    // -- Operators

    Projection& operator=(const Projection&) = default;
    Projection& operator=(Projection&&)      = default;

    // -- Methods

    virtual Point fwd(const Point&) const = 0;
    virtual Point inv(const Point&) const = 0;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
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


}  // namespace grit
