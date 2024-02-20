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
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "eckit/geo/Area.h"
#include "eckit/geo/Increments.h"
#include "eckit/geo/Iterator.h"
#include "eckit/geo/Ordering.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Renumber.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/geo/spec/Generator.h"
#include "eckit/memory/Builder.h"
#include "eckit/memory/Factory.h"


namespace eckit {
class JSON;
namespace geo {
class Spec;
}
}  // namespace eckit


namespace eckit::geo {


class Grid {
public:
    // -- Types

    using uid_t     = std::string;
    using builder_t = BuilderT1<Grid>;
    using ARG1      = const Spec&;

    struct Iterator final : std::unique_ptr<geo::Iterator> {
        explicit Iterator(geo::Iterator* it) :
            unique_ptr(it) {
            ASSERT(unique_ptr::operator bool());
        }

        using diff_t = unique_ptr::element_type::diff_t;

        Iterator(const Iterator&) = delete;
        Iterator(Iterator&&)      = delete;

        ~Iterator() = default;

        void operator=(const Iterator&) = delete;
        void operator=(Iterator&&)      = delete;

        bool operator==(const Iterator& other) const { return get()->operator==(*(other.get())); }
        bool operator!=(const Iterator& other) const { return get()->operator!=(*(other.get())); }

        bool operator++() { return get()->operator++(); }
        bool operator+=(diff_t d) { return get()->operator+=(d); }

        bool operator--() { return get()->operator--(); }
        bool operator-=(diff_t d) { return get()->operator-=(d); }

        explicit operator bool() const { return get()->operator bool(); }
        Point operator*() const { return get()->operator*(); }

        size_t index() const { return get()->index(); }
    };

    using iterator = Iterator;

    // -- Exceptions
    // None

    // -- Constructors

    explicit Grid(const Spec&);

    Grid(const Grid&) = delete;
    Grid(Grid&&)      = delete;

    // -- Destructor

    virtual ~Grid() = default;

    // -- Convertors
    // None

    // -- Operators

    Grid& operator=(const Grid&) = delete;
    Grid& operator=(Grid&&)      = delete;

    // -- Methods

    iterator begin() const { return cbegin(); }
    iterator end() const { return cend(); }

    std::string spec() const;

    virtual iterator cbegin() const = 0;
    virtual iterator cend() const   = 0;

    virtual const area::BoundingBox& boundingBox() const;

    virtual size_t size() const;
    virtual uid_t uid() const;

    virtual bool includesNorthPole() const;
    virtual bool includesSouthPole() const;
    virtual bool isPeriodicWestEast() const;

    virtual std::vector<Point> to_points() const;
    virtual std::pair<std::vector<double>, std::vector<double>> to_latlon() const;

    virtual Ordering order() const;
    virtual Renumber reorder(Ordering) const;
    virtual Grid* grid_reorder(Ordering) const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods

    static std::string className() { return "grid"; }

protected:
    // -- Constructors

    explicit Grid(const area::BoundingBox&);

    // -- Methods

    area::BoundingBox bbox() const { return bbox_; }
    void bbox(const area::BoundingBox& bbox) { bbox_ = bbox; }

    static Renumber no_reorder(size_t size);

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    area::BoundingBox bbox_;

    // -- Methods

    virtual void json(JSON&) const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    friend bool operator==(const Grid& a, const Grid& b) { return a.spec() == b.spec(); }
};


using GridFactoryType = Factory<Grid>;
using SpecByName      = spec::GeneratorT<spec::SpecGeneratorT1<const std::string&>>;
using SpecByUID       = spec::GeneratorT<spec::SpecGeneratorT0>;


template <typename T>
using GridRegisterType = ConcreteBuilderT1<Grid, T>;

template <typename T>
using GridRegisterUID = spec::ConcreteSpecGeneratorT0<T>;

template <typename T>
using GridRegisterName = spec::ConcreteSpecGeneratorT1<T, const std::string&>;


struct GridFactory {
    // This is 'const' as Grid should always be immutable
    static const Grid* build(const Spec& spec) { return instance().build_(spec); }

    // This is 'const' as Grid should always be immutable
    static const Grid* make_from_string(const std::string&);

    static Spec* spec(const Spec& spec) { return instance().generate_spec_(spec); }
    static void list(std::ostream& out) { return instance().list_(out); }

private:
    static GridFactory& instance();

    // This is 'const' as Grid should always be immutable
    const Grid* build_(const Spec&) const;

    Spec* generate_spec_(const Spec&) const;
    void list_(std::ostream&) const;
};


}  // namespace eckit::geo
