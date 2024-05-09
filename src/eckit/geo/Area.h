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


namespace eckit::geo {
class Spec;
namespace spec {
class Custom;
}
}  // namespace eckit::geo


namespace eckit::geo {


class Area {
public:
    // -- Types

    using builder_t = BuilderT1<Area>;
    using ARG1      = const Spec&;

    // -- Constructors

    Area() noexcept   = default;
    Area(const Area&) = default;
    Area(Area&&)      = default;

    // -- Destructor

    virtual ~Area() = default;

    // -- Operators

    Area& operator=(const Area&) = default;
    Area& operator=(Area&&)      = default;

    // -- Methods

    static std::string className() { return "area"; }

    virtual void spec(spec::Custom&) const;
};


// using AreaFactory = Factory<Area>;

// template <typename T>
// using AreaBuilder = ConcreteBuilderT1<Area, T>;


}  // namespace eckit::geo
