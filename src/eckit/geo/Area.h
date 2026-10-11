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

    /// Spec on its own
    virtual void fill_spec(spec::Custom&) const = 0;

    /// Spec as part of a grid's: nothing for the default area, "area": [n, w, s, e] for a bounding box, "area": {...}
    /// otherwise
    void fill_grid_spec(spec::Custom&) const;

    virtual const std::string& type() const = 0;

    bool is_default() const;

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

    friend bool operator==(const Area& a, const Area& b) { return a.spec_str() == b.spec_str(); }
    friend bool operator!=(const Area& a, const Area& b) { return !(a == b); }
};


// a type of its own, so names aren't shared with other registries (e.g. grids')
struct AreaSpecGenerator : spec::SpecGeneratorT1<const std::string&> {};

using AreaSpecByName = spec::GeneratorT<AreaSpecGenerator>;


template <typename T>
using AreaRegisterType = ConcreteBuilderT1<Area, T>;


struct AreaFactory {
    [[nodiscard]] static const Area* build(const Area::Spec&);
    [[nodiscard]] static const Area* make_from_string(const std::string&);
    [[nodiscard]] static Area::Spec* make_spec(const Area::Spec&);

    static void add_library(const std::string& lib, Area::Spec*);

    static std::ostream& list(std::ostream&);
};


}  // namespace eckit::geo
