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
#include "eckit/geo/Iterator.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/memory/Builder.h"
#include "eckit/memory/Factory.h"
#include "eckit/spec/Custom.h"
#include "eckit/spec/Generator.h"


namespace eckit {
class JSON;
namespace geo {
class Area;
}
}  // namespace eckit


namespace eckit::geo {


class Grid {
public:

    // -- Types

    using uid_type      = std::string;
    using order_type    = std::string;
    using renumber_type = std::vector<size_t>;

    using BoundingBox = area::BoundingBox;

    using builder_t = BuilderT1<Grid>;
    using Spec      = spec::Spec;
    using ARG1      = const Spec&;

    struct Iterator final : std::shared_ptr<geo::Iterator> {
        explicit Iterator(geo::Iterator* it) : shared_ptr(it) { ASSERT(shared_ptr::operator bool()); }

        using iterator_category = element_type::iterator_category;
        using difference_type   = element_type::difference_type;
        using value_type        = element_type::value_type;
        using pointer           = element_type::pointer;
        using reference         = element_type::reference;

        bool operator==(const Iterator& other) const { return get()->operator==(*(other.get())); }
        bool operator!=(const Iterator& other) const { return get()->operator!=(*(other.get())); }

        bool operator++() { return get()->operator++(); }
        bool operator+=(difference_type d) { return get()->operator+=(d); }

        bool operator--() { return get()->operator--(); }
        bool operator-=(difference_type d) { return get()->operator-=(d); }

        explicit operator bool() const { return get()->operator bool(); }
        Point operator*() const { return get()->operator*(); }

        size_t index() const { return get()->index(); }
    };

    using iterator = Iterator;

    // -- Constructors

    Grid(const Grid&) = delete;
    Grid(Grid&&)      = delete;

    // -- Destructor

    virtual ~Grid() = default;

    // -- Operators

    Grid& operator=(const Grid&) = delete;
    Grid& operator=(Grid&&)      = delete;

    // -- Methods

    iterator begin() const { return cbegin(); }
    iterator end() const { return cend(); }

    virtual iterator cbegin() const = 0;
    virtual iterator cend() const   = 0;

    [[nodiscard]] const spec::Spec& catalog() const;
    [[nodiscard]] const Spec& spec() const;
    std::string spec_str() const { return spec().str(); }

    virtual const std::string& type() const   = 0;
    virtual std::vector<size_t> shape() const = 0;

    virtual bool empty() const;
    virtual size_t size() const;

    uid_type uid() const;
    [[nodiscard]] virtual uid_type calculate_uid() const;

    static bool is_uid(const std::string& uid);

    virtual bool includesNorthPole() const;
    virtual bool includesSouthPole() const;
    virtual bool isPeriodicWestEast() const;

    [[nodiscard]] virtual Point first_point() const;
    [[nodiscard]] virtual Point last_point() const;
    [[nodiscard]] virtual std::vector<Point> to_points() const;
    [[nodiscard]] virtual std::pair<std::vector<double>, std::vector<double>> to_latlons() const;

    virtual const order_type& order() const;
    virtual renumber_type reorder(const order_type&) const;

    virtual const Area& area() const;
    virtual renumber_type crop(const Area&) const;

    virtual const Projection& projection() const;

    virtual const BoundingBox& boundingBox() const;
    [[nodiscard]] virtual BoundingBox* calculate_bbox() const;

    [[nodiscard]] virtual Grid* make_grid_reordered(const order_type&) const;
    [[nodiscard]] virtual Grid* make_grid_cropped(const Area&) const;

    // -- Class methods

    static std::string className() { return "grid"; }

protected:

    // -- Constructors

    explicit Grid(Projection* = nullptr);

    // -- Methods

    virtual void fill_spec(spec::Custom&) const;

    void reset_uid(uid_type = {});

    void projection(Projection* ptr) { projection_.reset(ptr); }
    void boundingBox(BoundingBox* bbox) { bbox_.reset(bbox); }

private:

    // -- Members

    mutable std::unique_ptr<const BoundingBox> bbox_;
    mutable std::unique_ptr<const Projection> projection_;
    mutable std::unique_ptr<const Spec> catalog_;
    mutable std::unique_ptr<spec::Custom> spec_;
    mutable uid_type uid_;

    // -- Friends

    friend bool operator==(const Grid& a, const Grid& b) { return a.spec_str() == b.spec_str(); }
    friend bool operator!=(const Grid& a, const Grid& b) { return !(a == b); }
};


struct GridSpecByName {
    using key_t                = std::string;
    using generator_t          = spec::GeneratorT<spec::SpecGeneratorT1<const key_t&>>;
    using concrete_generator_t = generator_t::generator_t;

    static generator_t& instance();
    static void regist(const key_t& key, concrete_generator_t* gen) { generator_t::instance().regist(key, gen); }
    static void unregist(const key_t& key) { generator_t::instance().unregist(key); }
};


struct GridSpecByUID {
    using key_t                = Grid::uid_type;
    using generator_t          = spec::GeneratorT<spec::SpecGeneratorT0>;
    using concrete_generator_t = generator_t::generator_t;

    static generator_t& instance();
    static void regist(const key_t& key, concrete_generator_t* gen) { generator_t::instance().regist(key, gen); }
    static void unregist(const key_t& key) { generator_t::instance().unregist(key); }
};


template <typename T>
using GridRegisterType = ConcreteBuilderT1<Grid, T>;

template <typename T>
using GridRegisterUID = spec::ConcreteSpecGeneratorT0<T>;

template <typename T>
bool GridRegisterName(const std::string& name_or_pattern) {
    new eckit::spec::ConcreteSpecGeneratorT1<T, const std::string&>(name_or_pattern);
    return true;
};


struct GridFactory {
    // This is 'const' as Grid should always be immutable
    [[nodiscard]] static const Grid* build(const Grid::Spec& spec) { return instance().make_from_spec_(spec); }

    // This is 'const' as Grid should always be immutable
    [[nodiscard]] static const Grid* make_from_string(const std::string&);

    [[nodiscard]] static Grid::Spec* make_spec(const Grid::Spec& spec) { return instance().make_spec_(spec); }
    static std::ostream& list(std::ostream& out) { return instance().list_(out); }

private:

    static GridFactory& instance();

    // This is 'const' as Grid should always be immutable
    [[nodiscard]] const Grid* make_from_spec_(const Grid::Spec&) const;

    [[nodiscard]] Grid::Spec* make_spec_(const Grid::Spec&) const;
    std::ostream& list_(std::ostream&) const;
};


}  // namespace eckit::geo
