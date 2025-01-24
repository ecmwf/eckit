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
namespace area {
class BoundingBox;
}
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

    Area() noexcept = default;

    Area(const Area&) = default;
    Area(Area&&)      = default;

    // -- Destructor

    virtual ~Area() = default;

    // -- Operators

    Area& operator=(const Area&) = default;
    Area& operator=(Area&&)      = default;

    // -- Methods

    [[nodiscard]] spec::Custom* spec() const;
    std::string spec_str() const;

    virtual const std::string& type() const = 0;

    virtual bool intersects(area::BoundingBox&) const = 0;

    // -- Class methods

    static std::string className() { return "area"; }

private:
    // -- Methods

    virtual void fill_spec(spec::Custom&) const = 0;

    // -- Friends

    friend class Grid;
};


// using AreaFactory = Factory<Area>;

// template <typename T>
// using AreaBuilder = ConcreteBuilderT1<Area, T>;


}  // namespace eckit::geo
