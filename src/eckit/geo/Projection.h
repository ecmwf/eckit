// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/Point.h"
#include "eckit/memory/Builder.h"
#include "eckit/memory/Factory.h"
#include "eckit/spec/Custom.h"
#include "eckit/spec/Generator.h"


namespace eckit::geo {
class Figure;
}


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
    virtual Point from_grid_xy(double x, double y) const { return inv(PointXY{x, y}); }

    void falseXY(const PointXY& falseXY) { false_ = falseXY; }
    const PointXY& falseXY() const { return false_; }

    const Figure& figure() const { return *figure_; }

    virtual void fill_spec(spec::Custom&) const;
    virtual const std::string& type() const = 0;

    [[nodiscard]] const Spec& spec() const;
    std::string spec_str() const { return spec().str(); }
    std::string proj_str() const;

    bool is_default() const;

    // -- Class methods

    static std::string className() { return "projection"; }

    static const Projection& projection_default();

protected:

    // -- Methods

    /// Set the source/target point types, given by example points
    void point_types(const Point& source, const Point& target);

    /// Set the source/target point types as the source of the first and the target of the last projections
    void point_types_from(const Projection& first, const Projection& last);

    /// Swap the source/target point types
    void reverse_point_types();

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
    PointXY false_;
    size_t source_;  // point types (as Point alternatives)
    size_t target_;

    // -- Friends

    friend bool operator==(const Projection& a, const Projection& b) { return a.spec_str() == b.spec_str(); }
    friend bool operator!=(const Projection& a, const Projection& b) { return !(a == b); }
};


using ProjectionFactoryType = Factory<Projection>;
using ProjectionSpecByName  = spec::GeneratorT<spec::SpecGeneratorT1<const std::string&>>;


template <typename T>
using ProjectionRegisterType = ConcreteBuilderT1<Projection, T>;

template <typename T>
using ProjectionRegisterName = spec::ConcreteSpecGeneratorT1<T, const std::string&>;


struct ProjectionFactory {
    // This is 'const' as Projection should always be immutable
    [[nodiscard]] static const Projection* build(const Projection::Spec& spec) {
        return instance().make_from_spec_(spec);
    }

    [[nodiscard]] static const Projection* make_default();

    // This is 'const' as Projection should always be immutable
    [[nodiscard]] static const Projection* make_from_string(const std::string&);

    [[nodiscard]] static Projection::Spec* make_spec(const Projection::Spec& spec) {
        return instance().make_spec_(spec);
    }
    static std::ostream& list(std::ostream& out) { return instance().list_(out); }
    static bool has_type(const std::string& type) { return ProjectionFactoryType::instance().exists(type); }

private:

    static ProjectionFactory& instance();

    // This is 'const' as Projection should always be immutable
    [[nodiscard]] const Projection* make_from_spec_(const Projection::Spec&) const;

    [[nodiscard]] Projection::Spec* make_spec_(const Projection::Spec&) const;
    std::ostream& list_(std::ostream&) const;
};


}  // namespace eckit::geo
