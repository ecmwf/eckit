// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <string>

#include "eckit/memory/Builder.h"


namespace eckit {
namespace geo {
namespace area {
class BoundingBox;
}
class Projection;
}  // namespace geo
namespace spec {
class Custom;
class Spec;
}  // namespace spec
}  // namespace eckit


namespace eckit::geo {


/**
 * @brief Figure: describe a combination of "shape" (sphere, ellipsoid, geoid) and "size" (radius, a, b, elevation)
 */
class Figure {
public:

    // -- Types

    using builder_t = BuilderT0<Figure>;
    using Spec      = spec::Spec;

    // -- Constructors

    Figure() noexcept     = default;
    Figure(const Figure&) = delete;
    Figure(Figure&&)      = delete;

    explicit Figure(const Spec&);

    // -- Destructor

    virtual ~Figure() = default;

    // -- Operators

    Figure& operator=(const Figure&) = delete;
    Figure& operator=(Figure&&)      = delete;

    // -- Methods

    static std::string className() { return "figure"; }

    virtual double R() const;
    virtual double a() const;
    virtual double b() const;

    /// Surface area [L^2]
    virtual double area() const;

    /// Surface area between parallels and meridians [L^2]
    virtual double area(const area::BoundingBox&) const;

    [[nodiscard]] spec::Custom* spec() const;
    std::string spec_str() const;
    std::string proj_str() const;

    bool spherical() const;
    double eccentricity() const;
    double flattening() const;

    virtual bool is_default() const;

private:

    // -- Methods

    virtual void fill_spec(spec::Custom&) const;

    // -- Friends

    friend bool operator==(const Figure& a, const Figure& b);
    friend bool operator!=(const Figure& a, const Figure& b) { return !(a == b); }

    friend class Grid;
    friend class Projection;
};


struct FigureFactory {
    /// Build the spec's figure, or the default one if the spec doesn't describe any
    [[nodiscard]] static Figure* build(const Figure::Spec&);
    [[nodiscard]] static Figure* make_from_string(const std::string&);
    [[nodiscard]] static const Figure* make_default();
};


template <typename T>
using FigureRegisterType = ConcreteBuilderT0<Figure, T>;


}  // namespace eckit::geo
