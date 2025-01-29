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

#include <memory>
#include <string>

#include "eckit/geo/Point.h"
#include "eckit/memory/Builder.h"
#include "eckit/memory/Factory.h"


namespace eckit::geo {
class Figure;
class Spec;
namespace spec {
class Custom;
}
}  // namespace eckit::geo


namespace eckit::geo {


class Projection {
public:
    // -- Types

    using builder_t = BuilderT1<Projection>;
    using ARG1      = const Spec&;

    // -- Constructors

    Projection() noexcept         = default;
    Projection(const Projection&) = default;
    Projection(Projection&&)      = default;

    // -- Destructor

    virtual ~Projection() = default;

    // -- Operators

    Projection& operator=(const Projection&) = default;
    Projection& operator=(Projection&&)      = default;

    // -- Methods

    virtual Point fwd(const Point&) const = 0;
    virtual Point inv(const Point&) const = 0;

    [[nodiscard]] virtual Figure* make_figure() const;
    const Figure& figure() const;

    virtual const std::string& type() const = 0;

    [[nodiscard]] spec::Custom* spec() const;
    std::string spec_str() const;
    std::string proj_str() const;

    // -- Class methods

    static std::string className() { return "projection"; }

    [[nodiscard]] static Projection* make_from_spec(const Spec&);

private:
    // -- Members

    mutable std::shared_ptr<Figure> figure_;

    // -- Methods

    virtual void fill_spec(spec::Custom&) const;

    // -- Friends

    friend class Grid;

    friend bool operator==(const Projection& a, const Projection& b) { return a.spec_str() == b.spec_str(); }
    friend bool operator!=(const Projection& a, const Projection& b) { return !(a == b); }
};


using ProjectionFactory = Factory<Projection>;

template <typename T>
using ProjectionBuilder = ConcreteBuilderT1<Projection, T>;


}  // namespace eckit::geo
