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

#include <map>
#include <memory>
#include <string>

#include "eckit/geo/Point.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/spec/Generator.h"
#include "eckit/memory/Builder.h"
#include "eckit/memory/Factory.h"


namespace eckit::geo::area {
class BoundingBox;
}


namespace eckit::geo {


class Area {
public:

    // -- Types

    using builder_t = BuilderT1<Area>;
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

    virtual const std::string& type() const = 0;

    virtual bool intersects(area::BoundingBox&) const;
    virtual bool contains(const Point&) const;
    virtual double area() const;

    // -- Class methods

    static std::string className() { return "area"; }

private:

    // -- Members

    mutable std::shared_ptr<spec::Custom> spec_;

    // -- Methods

    virtual void fill_spec(spec::Custom&) const = 0;

    // -- Friends

    friend class Grid;

    // -- Friends

    friend bool operator==(const Area& a, const Area& b) { return a.spec_str() == b.spec_str(); }
    friend bool operator!=(const Area& a, const Area& b) { return !(a == b); }
};


using AreaFactoryType = Factory<Area>;
using AreaSpecByName  = spec::GeneratorT<spec::SpecGeneratorT1<const std::string&>>;


template <typename T>
using AreaRegisterType = ConcreteBuilderT1<Area, T>;

template <typename T>
using AreaRegisterName = spec::ConcreteSpecGeneratorT1<T, const std::string&>;


struct AreaFactory {
    [[nodiscard]] static const Area* build(const Spec& spec) { return instance().make_from_spec_(spec); }

    [[nodiscard]] static const Area* make_from_string(const std::string&);
    [[nodiscard]] static Spec* make_spec(const Spec& spec) { return instance().make_spec_(spec); }

    static void add_library(const std::string& lib, Spec* spec) { return instance().add_library_(lib, spec); }

    static std::ostream& list(std::ostream& out) { return instance().list_(out); }

private:

    static AreaFactory& instance();

    [[nodiscard]] const Area* make_from_spec_(const Spec&) const;
    [[nodiscard]] Spec* make_spec_(const Spec&) const;

    void add_library_(const std::string& lib, Spec* spec);

    std::ostream& list_(std::ostream&) const;

    std::map<std::string, std::unique_ptr<Spec>> libraries_;
};


}  // namespace eckit::geo
