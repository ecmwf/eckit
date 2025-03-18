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
#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/spec/Generator.h"
#include "eckit/memory/Builder.h"
#include "eckit/memory/Factory.h"


namespace eckit::geo {
class Figure;
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

    [[nodiscard]] const Spec& spec() const;
    std::string spec_str() const { return spec().str(); }
    std::string proj_str() const;

    // -- Class methods

    static std::string className() { return "projection"; }

    [[nodiscard]] static Projection* make_from_spec(const Spec&);

private:

    // -- Members

    mutable std::shared_ptr<Figure> figure_;
    mutable std::shared_ptr<spec::Custom> spec_;

    // -- Methods

    virtual void fill_spec(spec::Custom&) const = 0;

    // -- Friends

    friend class Grid;

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
    [[nodiscard]] static const Projection* build(const Spec& spec) { return instance().make_from_spec_(spec); }

    // This is 'const' as Projection should always be immutable
    [[nodiscard]] static const Projection* make_from_string(const std::string&);

    [[nodiscard]] static Spec* make_spec(const Spec& spec) { return instance().make_spec_(spec); }
    static std::ostream& list(std::ostream& out) { return instance().list_(out); }

private:

    static ProjectionFactory& instance();

    // This is 'const' as Projection should always be immutable
    [[nodiscard]] const Projection* make_from_spec_(const Spec&) const;

    [[nodiscard]] Spec* make_spec_(const Spec&) const;
    std::ostream& list_(std::ostream&) const;
};


}  // namespace eckit::geo
