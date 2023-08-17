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

#include "eckit/config/MappedConfiguration.h"
#include "eckit/geometry/Area.h"
#include "eckit/geometry/Increments.h"
#include "eckit/geometry/Iterator.h"
#include "eckit/geometry/Ordering.h"
#include "eckit/geometry/Point.h"
#include "eckit/geometry/Projection.h"
#include "eckit/geometry/Renumber.h"
#include "eckit/geometry/area/BoundingBox.h"


namespace eckit {
class JSON;
}


namespace eckit::geometry {


class Grid {
public:
    // -- Types

    struct Iterator final : std::unique_ptr<geometry::Iterator> {
        explicit Iterator(geometry::Iterator* it) :
            unique_ptr(it) { ASSERT(unique_ptr::operator bool()); }
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

    explicit Grid(const Configuration&);

    Grid(const Grid&) = delete;
    Grid(Grid&&)      = delete;

    // -- Destructor

    virtual ~Grid() = default;

    // -- Convertors
    // None

    // -- Operators
    // None

    Grid& operator=(const Grid&) = delete;
    Grid& operator=(Grid&&)      = delete;

    // -- Methods

    iterator begin() const { return cbegin(); }
    iterator end() const { return cend(); }

    virtual iterator cbegin() const = 0;
    virtual iterator cend() const   = 0;

    virtual const area::BoundingBox& boundingBox() const;
    void boundingBox(const area::BoundingBox&);

    virtual size_t size() const;

    virtual bool includesNorthPole() const;
    virtual bool includesSouthPole() const;
    virtual bool isPeriodicWestEast() const;

    virtual std::vector<Point> to_points() const;
    virtual std::pair<std::vector<double>, std::vector<double>> to_latlon() const;

    virtual Ordering order() const;
    virtual Renumber reorder(const PointLonLat&) const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
    // -- Constructors

    explicit Grid(const area::BoundingBox&);

    // -- Methods

    area::BoundingBox bbox() const { return bbox_; }
    void bbox(const area::BoundingBox& bbox) { bbox_ = bbox; }

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
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


struct GridFactory {
    // This is 'const' as Grid should always be immutable
    static const Grid* build(const Configuration&);
    static void list(std::ostream&);
    static void json(JSON&);
};


struct GridFactoryUID {
    GridFactoryUID(const GridFactoryUID&)            = delete;
    GridFactoryUID(GridFactoryUID&&)                 = delete;
    GridFactoryUID& operator=(const GridFactoryUID&) = delete;
    GridFactoryUID& operator=(GridFactoryUID&&)      = delete;

    // This is 'const' as Grid should always be immutable
    static const Grid* build(const std::string&);

    static void list(std::ostream&);
    static void json(JSON&);
    static void insert(const std::string& name, MappedConfiguration*);

protected:
    explicit GridFactoryUID(const std::string& uid);
    virtual ~GridFactoryUID();

private:
    virtual Configuration* config() = 0;

    const std::string uid_;
};


struct GridFactoryName {
    GridFactoryName(const GridFactoryName&)            = delete;
    GridFactoryName(GridFactoryName&&)                 = delete;
    GridFactoryName& operator=(const GridFactoryName&) = delete;
    GridFactoryName& operator=(GridFactoryName&&)      = delete;

    // This is 'const' as Grid should always be immutable
    static const Grid* build(const std::string& name);

    static void list(std::ostream&);
    static void json(JSON&);
    static void insert(const std::string& name, MappedConfiguration*);

protected:
    explicit GridFactoryName(const std::string& pattern, const std::string& example = "");
    virtual ~GridFactoryName();

private:
    Configuration* config() const { return config(example_); }
    virtual Configuration* config(const std::string& name) const = 0;

    const std::string pattern_;
    const std::string example_;
};


struct GridFactoryType {
    GridFactoryType(const GridFactoryType&)            = delete;
    GridFactoryType(GridFactoryType&&)                 = delete;
    GridFactoryType& operator=(const GridFactoryType&) = delete;
    GridFactoryType& operator=(GridFactoryType&&)      = delete;

    // This is 'const' as Grid should always be immutable
    static const Grid* build(const Configuration&);

    static void list(std::ostream&);
    static void json(JSON&);

protected:
    explicit GridFactoryType(const std::string& type);
    virtual ~GridFactoryType();

private:
    virtual const Grid* make(const Configuration&) = 0;

    const std::string type_;
};


template <class T>
struct GridRegisterUID final : GridFactoryUID {
    explicit GridRegisterUID(const std::string& uid) :
        GridFactoryUID(uid) {}
    Configuration* config() override { return T::config(); }
};


template <class T>
struct GridRegisterName final : GridFactoryName {
    explicit GridRegisterName(const std::string& pattern, const std::string& example) :
        GridFactoryName(pattern, example) {}
    Configuration* config(const std::string& name) const override { return T::config(name); }
};


template <class T>
struct GridRegisterType final : GridFactoryType {
    explicit GridRegisterType(const std::string& type) :
        GridFactoryType(type) {}
    const Grid* make(const Configuration& config) override { return new T(config); }
};


}  // namespace eckit::geometry
