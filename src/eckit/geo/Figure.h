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

#include <iosfwd>
#include <string>

#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/PointXYZ.h"
#include "eckit/memory/Builder.h"
#include "eckit/memory/Factory.h"


namespace eckit::geo {
namespace area {
class BoundingBox;
}
namespace projection {
class ProjectionOnFigure;
}
namespace spec {
class Custom;
}
class Spec;
}  // namespace eckit::geo


namespace eckit::geo {


/**
 * @brief Figure: describe a combination of "shape" (sphere, ellipsoid, geoid) and "size" (radius, a, b, elevation)
 */
class Figure {
public:

    // -- Types

    using builder_t = BuilderT0<Figure>;

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
    virtual double area(const area::BoundingBox&);

    [[nodiscard]] spec::Custom* spec() const;
    std::string spec_str() const;
    std::string proj_str() const;

    double eccentricity() const;
    double flattening() const;

private:

    // -- Methods

    virtual void fill_spec(spec::Custom&) const;

    // -- Friends

    friend bool operator==(const Figure& a, const Figure& b) { return a.spec_str() == b.spec_str(); }
    friend bool operator!=(const Figure& a, const Figure& b) { return !(a == b); }

    friend class projection::ProjectionOnFigure;
};


struct FigureFactory {
    [[nodiscard]] static Figure* build(const Spec& spec) { return instance().make_from_spec_(spec); }
    [[nodiscard]] static Figure* make_from_string(const std::string&);

private:

    static FigureFactory& instance();

    [[nodiscard]] Figure* make_from_spec_(const Spec&) const;
};


template <typename T>
using FigureBuilder = ConcreteBuilderT0<Figure, T>;


}  // namespace eckit::geo
