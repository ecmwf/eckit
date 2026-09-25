// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <iosfwd>
#include <string>

#include "eckit/geo/Point.h"
#include "eckit/memory/Builder.h"
#include "eckit/spec/Custom.h"
#include "eckit/spec/Generator.h"
#include "eckit/spec/Spec.h"


namespace eckit::geo::area {
class BoundingBox;
}


namespace eckit::geo {


class Area {
public:

    // -- Types

    using builder_t = BuilderT1<Area>;
    using Spec      = spec::Spec;
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

    [[nodiscard]] const Spec& spec() const;
    std::string spec_str() const { return spec().str(); }

    virtual void fill_spec(spec::Custom&) const = 0;
    virtual const std::string& type() const     = 0;

    virtual bool intersects(area::BoundingBox&) const;
    virtual bool contains(const Point&) const;
    virtual double area() const;

    // -- Class methods

    static std::string className() { return "area"; }

    static const Area& area_default();

private:

    // -- Members

    mutable std::shared_ptr<spec::Custom> spec_;

    // -- Friends

    friend class Grid;

    // -- Friends

    friend bool operator==(const Area& a, const Area& b) { return a.spec_str() == b.spec_str(); }
    friend bool operator!=(const Area& a, const Area& b) { return !(a == b); }
};


using AreaSpecByName = spec::GeneratorT<spec::SpecGeneratorT1<const std::string&>>;


template <typename T>
using AreaRegisterType = ConcreteBuilderT1<Area, T>;

template <typename T>
using AreaRegisterName = spec::ConcreteSpecGeneratorT1<T, const std::string&>;


struct AreaFactory {
    [[nodiscard]] static const Area* build(const Area::Spec&);
    [[nodiscard]] static const Area* make_from_string(const std::string&);
    [[nodiscard]] static Area::Spec* make_spec(const Area::Spec&);

    static void add_library(const std::string& lib, Area::Spec*);

    static std::ostream& list(std::ostream&);
};


}  // namespace eckit::geo
