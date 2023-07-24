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
    // None

    // -- Exceptions
    // None

    // -- Constructors

    explicit Grid(const Configuration&);

    Grid(const Grid&) = default;
    Grid(Grid&&)      = default;

    // -- Destructor

    virtual ~Grid() = default;

    // -- Convertors
    // None

    // -- Operators
    // None

    Grid& operator=(const Grid&) = default;
    Grid& operator=(Grid&&)      = default;

    // -- Methods

    virtual const area::BoundingBox& boundingBox() const;

    virtual size_t size() const = 0;

    virtual void print(std::ostream&) const = 0;

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

    Grid(const area::BoundingBox&);

    // None

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

    friend std::ostream& operator<<(std::ostream& s, const Grid& p) {
        p.print(s);
        return s;
    }
};


static struct GridFactory {
    // This is 'const' as Grid should always be immutable
    static const Grid* build(const Configuration&);
    static void list(std::ostream&);
};


struct GridFactoryUID {
    explicit GridFactoryUID(const std::string& uid);
    virtual ~GridFactoryUID();

    virtual const Grid* make() = 0;

    GridFactoryUID(const GridFactoryUID&)            = delete;
    GridFactoryUID(GridFactoryUID&&)                 = delete;
    GridFactoryUID& operator=(const GridFactoryUID&) = delete;
    GridFactoryUID& operator=(GridFactoryUID&&)      = delete;

    // This is 'const' as Grid should always be immutable
    static const Grid* build(const std::string&);

    static void list(std::ostream&);

private:
    const std::string uid_;
};


struct GridFactoryName {
    explicit GridFactoryName(const std::string& name);
    virtual ~GridFactoryName();

    virtual const Grid* make(const Configuration&) = 0;

    GridFactoryName(const GridFactoryName&)            = delete;
    GridFactoryName(GridFactoryName&&)                 = delete;
    GridFactoryName& operator=(const GridFactoryName&) = delete;
    GridFactoryName& operator=(GridFactoryName&&)      = delete;

    // This is 'const' as Grid should always be immutable
    static const Grid* build(const std::string&, Area* = nullptr, Projection* = nullptr);
    static const Grid* build(const std::string&, Projection*);

    static void list(std::ostream&);

private:
    const std::string name_;
};


struct GridFactoryType {
    explicit GridFactoryType(const std::string& type);
    virtual ~GridFactoryType();

    virtual const Grid* make(const Configuration&) = 0;

    GridFactoryType(const GridFactoryType&)            = delete;
    GridFactoryType(GridFactoryType&&)                 = delete;
    GridFactoryType& operator=(const GridFactoryType&) = delete;
    GridFactoryType& operator=(GridFactoryType&&)      = delete;

    // This is 'const' as Grid should always be immutable
    static const Grid* build(const std::string&, Area* = nullptr, Projection* = nullptr);
    static const Grid* build(const std::string&, Projection*);

    static void list(std::ostream&);

private:
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
    explicit GridRegisterName(const std::string& name) :
        GridFactoryName(name) {}
    const Grid* make(const Configuration& config) override { return new T(config); }
};


template <class T>
struct GridRegisterType final : GridFactoryType {
    explicit GridRegisterType(const std::string& type) :
        GridFactoryType(type) {}
    const Grid* make(const Configuration& config) override { return new T(config); }
};


}  // namespace eckit::geometry