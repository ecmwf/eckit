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


class Transformation {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    Transformation() noexcept = default;

    Transformation(const Transformation&) = default;
    Transformation(Transformation&&)      = default;

    // -- Destructor

    virtual ~Transformation() = default;

    // -- Convertors
    // None

    // -- Operators

    Transformation& operator=(const Transformation&) = default;
    Transformation& operator=(Transformation&&)      = default;

    virtual Point operator()(const Point&) const = 0;

    // -- Methods
    // None

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
