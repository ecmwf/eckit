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
#include <vector>

#include "eckit/config/Configuration.h"
#include "eckit/geometry/Area.h"
#include "eckit/geometry/Increments.h"
#include "eckit/geometry/Point.h"
#include "eckit/geometry/Projection.h"
#include "eckit/geometry/Renumber.h"
#include "eckit/geometry/area/BoundingBox.h"


namespace eckit::geometry {


class Grid {
public:
    // -- Types

    struct Iterator {
        explicit Iterator(const Grid& grid) :
            grid_(grid) {}

        Iterator(const Iterator&) = delete;
        Iterator(Iterator&&)      = delete;

        virtual ~Iterator() = default;

        void operator=(const Iterator&) = delete;
        void operator=(Iterator&&)      = delete;

        bool operator!=(const Iterator& other) const { return &grid_ != &other.grid_ || index_ != other.index_; }

        bool operator++() {
            index_++;
            return operator bool();
        }

        virtual explicit operator bool()       = 0;
        virtual const Point& operator*() const = 0;

        size_t size() const { return grid_.size(); }
        size_t index() const { return index_; }

    private:
        const Grid& grid_;
        size_t index_ = 0;
    };


    using iterator       = std::unique_ptr<Iterator>;
    using const_iterator = std::unique_ptr<const Iterator>;

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

    virtual iterator begin() = 0;
    virtual iterator end()   = 0;

    virtual const_iterator cbegin() const = 0;
    virtual const_iterator cend() const   = 0;

    virtual const_iterator begin() const = 0;
    virtual const_iterator end() const   = 0;

    virtual const area::BoundingBox& boundingBox() const;

    virtual size_t size() const = 0;

    virtual bool includesNorthPole() const  = 0;
    virtual bool includesSouthPole() const  = 0;
    virtual bool isPeriodicWestEast() const = 0;

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
};


struct GridFactoryUID {
    GridFactoryUID(const GridFactoryUID&)            = delete;
    GridFactoryUID(GridFactoryUID&&)                 = delete;
    GridFactoryUID& operator=(const GridFactoryUID&) = delete;
    GridFactoryUID& operator=(GridFactoryUID&&)      = delete;

    // This is 'const' as Grid should always be immutable
    static const Grid* build(const std::string&);
    static void list(std::ostream&);

protected:
    explicit GridFactoryUID(const std::string& uid);
    virtual ~GridFactoryUID();

private:
    virtual const Grid* make() = 0;

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

protected:
    explicit GridFactoryName(const std::string& pattern);
    virtual ~GridFactoryName();

private:
    virtual Configuration* config(const std::string& name) = 0;

    const std::string pattern_;
};


struct GridFactoryType {
    GridFactoryType(const GridFactoryType&)            = delete;
    GridFactoryType(GridFactoryType&&)                 = delete;
    GridFactoryType& operator=(const GridFactoryType&) = delete;
    GridFactoryType& operator=(GridFactoryType&&)      = delete;

    // This is 'const' as Grid should always be immutable
    static const Grid* build(const Configuration&);
    static void list(std::ostream&);

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
    const Grid* make() override { return new T(); }
};


template <class T>
struct GridRegisterName final : GridFactoryName {
    explicit GridRegisterName(const std::string& pattern) :
        GridFactoryName(pattern) {}
    Configuration* config(const std::string& name) override { return T::config(name); }
};


template <class T>
struct GridRegisterType final : GridFactoryType {
    explicit GridRegisterType(const std::string& type) :
        GridFactoryType(type) {}
    const Grid* make(const Configuration& config) override { return new T(config); }
};


}  // namespace eckit::geometry
