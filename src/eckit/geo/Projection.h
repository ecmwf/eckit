// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <string>

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

    explicit Projection(Figure* = nullptr);
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

    [[nodiscard]] static Projection* make_from_spec(const Spec&);

private:

    // -- Members

    std::shared_ptr<const Figure> figure_;
    mutable std::shared_ptr<spec::Custom> spec_;
    PointXY false_;

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
