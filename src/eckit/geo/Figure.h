// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <set>
#include <string>

#include "eckit/memory/Builder.h"


namespace eckit {
namespace geo {
namespace area {
class BoundingBox;
}
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

    /**
     * @brief Functions of the latitude [radian] for conformal projections, on an ellipsoid of eccentricity e (a sphere
     * if e = 0), as PROJ (pj_msfn, pj_tsfn and pj_phi2)
     * @ref Map Projections: A Working Manual, John P. Snyder (1987)
     */
    class Conformal {
    public:

        explicit Conformal(double e) : e_(e) {}

        /// Snyder (14-15)
        double m(double phi) const;

        /// Snyder (15-9)
        double t(double phi) const;

        /// The latitude of t, Snyder (7-9)
        double phi(double t) const;

    private:

        const double e_;
    };

    /**
     * @brief Functions of the latitude [radian] for equal-area projections, on an ellipsoid of eccentricity e (a sphere
     * if e = 0), as PROJ (pj_qsfn and pj_authlat)
     * @ref Map Projections: A Working Manual, John P. Snyder (1987)
     */
    class Authalic {
    public:

        explicit Authalic(double e);

        /// Snyder (3-12)
        double q(double sinphi) const;

        /// The latitude of q, Snyder (3-16)
        double phi(double q) const;

        /// q at the poles
        double qp() const { return qp_; }

    private:

        const double e_;
        const double qp_;
    };

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
    double area() const;

    /// Surface area between parallels and meridians [L^2]
    double area(const area::BoundingBox&) const;

    [[nodiscard]] spec::Custom* spec() const;
    std::string spec_str() const;
    std::string proj_str() const;

    /// Spec on its own
    virtual void fill_spec(spec::Custom&) const;

    /// Spec as part of a projection's: nothing for the default figure
    void fill_projection_spec(spec::Custom&) const;

    bool spherical() const;
    double eccentricity() const;
    double flattening() const;

    Conformal conformal() const { return Conformal(eccentricity()); }
    Authalic authalic() const { return Authalic(eccentricity()); }

    virtual bool is_default() const;

private:

    // -- Friends

    friend bool operator==(const Figure& a, const Figure& b);
    friend bool operator!=(const Figure& a, const Figure& b) { return !(a == b); }
};


struct FigureFactory {
    /// Build the spec's figure, or the default one if the spec doesn't describe any
    [[nodiscard]] static Figure* build(const Figure::Spec&);
    [[nodiscard]] static Figure* make_from_string(const std::string&);
    [[nodiscard]] static const Figure* make_default();

    static const std::set<std::string>& keys();
};


template <typename T>
using FigureRegisterType = ConcreteBuilderT0<Figure, T>;


}  // namespace eckit::geo
