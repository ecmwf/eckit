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

    using Increments = Increments;
    using Projection = Projection;
    using Area       = Area;

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


class GridFactory {
    std::string name_;
    virtual Grid* make(const Configuration&) = 0;

protected:
    explicit GridFactory(const std::string&);
    virtual ~GridFactory();

public:
    // This is 'const' as Grid should always be immutable
    static const Grid* build(const Configuration&);
    static const Grid* build_from_uid(const std::string&);
    static const Grid* build_from_name(const std::string&, Grid::Area* = nullptr, Grid::Projection* = nullptr);
    static const Grid* build_from_name(const std::string&, Grid::Projection*);
    static const Grid* build_from_type(const std::string&, Grid::Area* = nullptr, Grid::Projection* = nullptr);
    static const Grid* build_from_type(const std::string&, Grid::Projection*);
    static const Grid* build_from_increments(const Increments&, Grid::Area* = nullptr, Grid::Projection* = nullptr);
    static const Grid* build_from_increments(const Increments&, Grid::Projection*);

    static void list(std::ostream&);

    GridFactory(const GridFactory&)            = delete;
    GridFactory(GridFactory&&)                 = delete;
    GridFactory& operator=(const GridFactory&) = delete;
    GridFactory& operator=(GridFactory&&)      = delete;
};


template <class T>
class GridBuilder : public GridFactory {
    Grid* make(const Configuration& config) override { return new T(config); }

public:
    GridBuilder(const std::string& name) :
        GridFactory(name) {}
};


}  // namespace eckit::geometry
