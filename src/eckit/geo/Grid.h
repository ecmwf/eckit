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
#include "eckit/geo/Projection.h"
#include "eckit/geo/Renumber.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/geo/projection/Rotation.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/spec/Generator.h"
#include "eckit/memory/Builder.h"
#include "eckit/memory/Factory.h"


namespace eckit {
class JSON;
namespace geo {
class Area;
}  // namespace geo
}  // namespace eckit


namespace eckit::geo {


class Grid {
public:
    // -- Types

    using uid_t     = std::string;
    using builder_t = BuilderT1<Grid>;
    using ARG1      = const Spec&;

    struct Iterator final : std::unique_ptr<geo::Iterator> {
        explicit Iterator(geo::Iterator* it) : unique_ptr(it) { ASSERT(unique_ptr::operator bool()); }

        using difference_type = unique_ptr::element_type::difference_type;

        Iterator(const Iterator&) = delete;
        Iterator(Iterator&&)      = delete;

        ~Iterator() = default;

        void operator=(const Iterator&) = delete;
        void operator=(Iterator&&)      = delete;

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

    class NextIterator final {
    public:
        NextIterator(const NextIterator&) = delete;
        NextIterator(NextIterator&&)      = delete;

        ~NextIterator() {
            delete current_;
            delete end_;
        }

        void operator=(const NextIterator&) = delete;
        void operator=(NextIterator&&)      = delete;

        bool next(Point&) const;
        bool has_next() const { return *current_ != *end_; }
        size_t index() const { return index_; }

    private:
        NextIterator(geo::Iterator* current, const geo::Iterator* end);

        geo::Iterator* current_;
        const geo::Iterator* end_;
        mutable size_t index_;

        friend class Grid;
    };

    // -- Constructors

    explicit Grid(const Spec&);

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

    NextIterator next_iterator() const { return {cbegin().release(), cend().release()}; }

    [[nodiscard]] NextIterator* make_next_iterator() const {
        return new NextIterator{cbegin().release(), cend().release()};
    }

    const Spec& spec() const;
    std::string spec_str() const { return spec().str(); }

    virtual const std::string& type() const   = 0;
    virtual std::vector<size_t> shape() const = 0;

    virtual size_t size() const;

    uid_t uid() const;
    [[nodiscard]] virtual uid_t calculate_uid() const;

    virtual bool includesNorthPole() const;
    virtual bool includesSouthPole() const;
    virtual bool isPeriodicWestEast() const;

    [[nodiscard]] virtual std::vector<Point> to_points() const;
    [[nodiscard]] virtual std::pair<std::vector<double>, std::vector<double>> to_latlons() const;

    virtual Ordering ordering() const;
    virtual Renumber reorder(Ordering) const;

    virtual const Area& area() const;
    virtual Renumber crop(const Area&) const;

    virtual const Projection& projection() const;

    virtual const area::BoundingBox& boundingBox() const;
    [[nodiscard]] virtual area::BoundingBox* calculate_bbox() const;

    [[nodiscard]] virtual Grid* make_grid_reordered(Ordering) const;
    [[nodiscard]] virtual Grid* make_grid_cropped(const Area&) const;

    // -- Class methods

    static std::string className() { return "grid"; }

protected:
    // -- Constructors

    explicit Grid(const area::BoundingBox&, Projection* = nullptr, Ordering = Ordering::DEFAULT);
    explicit Grid(Ordering = Ordering::DEFAULT);

    // -- Methods

    virtual void fill_spec(spec::Custom&) const;

    static Renumber no_reorder(size_t size);

    void reset_uid(uid_t = {});

    void area(Area* ptr) { area_.reset(ptr); }
    void projection(Projection* ptr) { projection_.reset(ptr); }

private:
    // -- Members

    mutable std::unique_ptr<Area> area_;
    mutable std::unique_ptr<area::BoundingBox> bbox_;
    mutable std::unique_ptr<Projection> projection_;
    mutable std::unique_ptr<spec::Custom> spec_;
    mutable uid_t uid_;

    Ordering ordering_;

    // -- Friends

    friend bool operator==(const Grid& a, const Grid& b) { return a.spec_str() == b.spec_str(); }
    friend bool operator!=(const Grid& a, const Grid& b) { return !(a == b); }
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
    [[nodiscard]] static const Grid* build(const Spec& spec) { return instance().make_from_spec_(spec); }

    // This is 'const' as Grid should always be immutable
    [[nodiscard]] static const Grid* make_from_string(const std::string&);

    [[nodiscard]] static Spec* make_spec(const Spec& spec) { return instance().make_spec_(spec); }
    static void list(std::ostream& out) { return instance().list_(out); }

private:
    static GridFactory& instance();

    // This is 'const' as Grid should always be immutable
    [[nodiscard]] const Grid* make_from_spec_(const Spec&) const;

    [[nodiscard]] Spec* make_spec_(const Spec&) const;
    void list_(std::ostream&) const;
};


}  // namespace eckit::geo
