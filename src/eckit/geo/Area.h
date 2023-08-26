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

#include <string>

#include "eckit/memory/Builder.h"
#include "eckit/memory/Factory.h"


namespace eckit {
class Configuration;
}


namespace eckit::geo {


class Area {
public:
    // -- Types

    using builder_t = BuilderT1<Area>;
    using ARG1      = const Configuration&;

    // -- Exceptions
    // None

    // -- Constructors

    Area() noexcept   = default;
    Area(const Area&) = default;
    Area(Area&&)      = default;

    // -- Destructor

    virtual ~Area() = default;

    // -- Convertors
    // None

    // -- Operators

    Area& operator=(const Area&) = default;
    Area& operator=(Area&&)      = default;

    // -- Methods

    static std::string className() { return "area"; }

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


// using AreaFactory = Factory<Area>;

// template <typename T>
// using AreaBuilder = ConcreteBuilderT1<Area, T>;


}  // namespace eckit::geo
