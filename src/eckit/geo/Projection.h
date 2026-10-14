// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/Figure.h"
#include "eckit/geo/Point.h"
#include "eckit/memory/Builder.h"
#include "eckit/spec/Custom.h"
#include "eckit/spec/Generator.h"
#include "eckit/spec/Spec.h"


namespace eckit::geo {


class Projection {
public:

    // -- Types

    using builder_t = BuilderT1<Projection>;
    using Spec      = spec::Spec;
    using ARG1      = const Spec&;

    // -- Constructors

    /// @param source/target point types, given by example points
    explicit Projection(Figure* = nullptr, const Point& source = PointLonLat{}, const Point& target = PointXY{});
    Projection(const Projection&) = default;
    Projection(Projection&&)      = default;

    // -- Destructor

    virtual ~Projection() = default;

    // -- Operators

    Projection& operator=(const Projection&) = default;
    Projection& operator=(Projection&&)      = default;

    // -- Methods

    /// Project a point, from the source to the target point type (fwd) or the reverse (inv)
    virtual Point fwd(const Point&) const = 0;
    virtual Point inv(const Point&) const = 0;

    /// Project points given one vector per coordinate (v1, v2 and, for points of 3 coordinates, v3), from the source
    /// to the target point type (fwd) or the reverse (inv), returning one vector per coordinate; points that fail to
    /// project result in NaN coordinates
    std::vector<std::vector<double>> fwd(const std::vector<double>& v1, const std::vector<double>& v2,
                                         const std::vector<double>& v3 = {}) const;
    std::vector<std::vector<double>> inv(const std::vector<double>& v1, const std::vector<double>& v2,
                                         const std::vector<double>& v3 = {}) const;

    /// Source/target point coordinate names (see point_coordinates)
    const std::vector<std::string>& source_point_coordinates() const;
    const std::vector<std::string>& target_point_coordinates() const;

    /// Map a grid's (x, y) coordinates to geographic coordinates
    Point from_grid_xy(double x, double y) const;

    const Figure& figure() const { return *figure_; }

    /// Spec on its own, with the figure unless the default
    virtual void fill_spec(spec::Custom&) const;

    /// Spec as part of a grid's: "projection": {...}, but for the default projection only its figure (as the grid's)
    void fill_grid_spec(spec::Custom&) const;

    virtual const std::string& type() const = 0;

    [[nodiscard]] const Spec& spec() const;
    std::string spec_str() const { return spec().str(); }
    std::string proj_str() const;

    bool is_default() const;

    /// If fwd/inv are reversed
    virtual bool reverse() const { return false; }

    /// If fwd/inv are the identity, the same reversed
    virtual bool identity() const { return false; }

    // -- Class methods

    static std::string className() { return "projection"; }

    /// Default projection, (lon, lat) to (x, y) in degrees (equidistant cylindrical)
    static const Projection& projection_default();

    /// PROJ string of a projection spec (and its figure), for the projection types with a PROJ equivalent (not
    /// requiring PROJ)
    static std::string proj_str(const spec::Custom&);

    /// PROJ default coordinate reference system, (lon, lat) on WGS84
    static const std::string& proj_default();

protected:

    // -- Constructors

    /// Share the figure of another projection, with its source/target point types (swapped, if reversing it)
    Projection(const Projection&, bool reverse);

    // -- Methods

    /// Set the source/target point types, given by example points
    void point_types(const Point& source, const Point& target);

    /// Set the source/target point types as the source of the first and the target of the last projections
    void point_types_from(const Projection& first, const Projection& last);

    /// Project points (see fwd/inv), given one vector per coordinate of checked sizes (v3 is empty for points of 2
    /// coordinates); the default projects point by point
    virtual std::vector<std::vector<double>> fwd_vector(const std::vector<double>& v1, const std::vector<double>& v2,
                                                        const std::vector<double>& v3) const;
    virtual std::vector<std::vector<double>> inv_vector(const std::vector<double>& v1, const std::vector<double>& v2,
                                                        const std::vector<double>& v3) const;

private:

    // -- Members

    std::shared_ptr<const Figure> figure_;
    mutable std::shared_ptr<spec::Custom> spec_;
    size_t source_;  // point types (as Point alternatives)
    size_t target_;

    // -- Friends

    friend bool operator==(const Projection& a, const Projection& b) { return a.spec_str() == b.spec_str(); }
    friend bool operator!=(const Projection& a, const Projection& b) { return !(a == b); }
};


// a type of its own, so names aren't shared with other registries (e.g. grids')
struct ProjectionSpecGenerator : spec::SpecGeneratorT1<const std::string&> {};

using ProjectionSpecByName = spec::GeneratorT<ProjectionSpecGenerator>;


struct ProjectionFactory {
    // This is 'const' as Projection should always be immutable
    [[nodiscard]] static const Projection* build(const Projection::Spec&);
    [[nodiscard]] static const Projection* make_from_string(const std::string&);
    [[nodiscard]] static const Projection* make_default();

    [[nodiscard]] static Projection::Spec* make_spec(const Projection::Spec&);
    static std::ostream& list(std::ostream&);
    static bool has_type(const std::string&);

    /// Projection types interpreting "reverse": true themselves, others are wrapped (see projection::Reverse)
    static void register_self_reversing(const std::string& type);
};


template <typename T>
class ProjectionRegisterType {
public:

    explicit ProjectionRegisterType(const std::string& type, bool self_reversing = false) : builder_(type) {
        if (self_reversing) {
            ProjectionFactory::register_self_reversing(type);
        }
    }

private:

    ConcreteBuilderT1<Projection, T> builder_;
};


}  // namespace eckit::geo
