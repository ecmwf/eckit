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


namespace eckit::geometry {


class Figure {
public:
    // -- Types

    using builder_t = BuilderT1<Figure>;
    using ARG1      = const Configuration&;

    // -- Exceptions
    // None

    // -- Constructors

    Figure() noexcept     = default;
    Figure(const Figure&) = default;
    Figure(Figure&&)      = default;

    // -- Destructor

    virtual ~Figure() = default;

    // -- Convertors
    // None

    // -- Operators

    Figure& operator=(const Figure&) = default;
    Figure& operator=(Figure&&)      = default;

    // -- Methods

    static std::string className() { return "figure"; }

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


using FigureFactory = Factory<Figure>;

template <typename T>
using FigureBuilder = ConcreteBuilderT1<Figure, T>;


}  // namespace eckit::geometry
