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

#include "eckit/geo/Point.h"
#include "eckit/memory/Builder.h"
#include "eckit/memory/Factory.h"


namespace eckit {
class Configuration;
}


namespace eckit::geo {


class Projection {
public:
    // -- Types

    using builder_t = BuilderT1<Projection>;
    using ARG1      = const Configuration&;

    // -- Exceptions
    // None

    // -- Constructors

    Projection() noexcept         = default;
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

    static std::string className() { return "projection"; }

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


using ProjectionFactory = Factory<Projection>;

template <typename T>
using ProjectionBuilder = ConcreteBuilderT1<Projection, T>;


}  // namespace eckit::geo
