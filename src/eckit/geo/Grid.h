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
#include "eckit/geo/BoundingBox.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Renumber.h"


namespace eckit::geo {
class Domain;
class Iterator;
}  // namespace eckit::geo


namespace eckit::geo {


class Grid {
public:
    // -- Exceptions
    // None

    // -- Constructors

    Grid(const Configuration&);
    Grid(const BoundingBox&);

    // -- Destructor

    virtual ~Grid() = default;

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    virtual Domain domain() const;
    virtual const BoundingBox& boundingBox() const;

    virtual Iterator* iterator() const      = 0;
    virtual size_t numberOfPoints() const   = 0;
    virtual void print(std::ostream&) const = 0;

    virtual bool includesNorthPole() const  = 0;
    virtual bool includesSouthPole() const  = 0;
    virtual bool isPeriodicWestEast() const = 0;

    virtual Renumber crop(BoundingBox&) const         = 0;
    virtual Renumber reorder(long scanningMode) const = 0;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
    // -- Members
    // None

    // -- Methods

    const BoundingBox& bbox() const { return bbox_; }
    void bbox(const BoundingBox& bbox) { bbox_ = bbox; }

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    BoundingBox bbox_;

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

    GridFactory(const GridFactory&)            = delete;
    GridFactory& operator=(const GridFactory&) = delete;

protected:
    GridFactory(const std::string&);
    virtual ~GridFactory();

public:
    // This is 'const' as Grid should always be immutable
    static const Grid* build(const Configuration&);
    static void list(std::ostream&);
};


template <class T>
class GridBuilder : public GridFactory {
    Grid* make(const Configuration& config) override { return new T(config); }

public:
    GridBuilder(const std::string& name) :
        GridFactory(name) {}
};


}  // namespace eckit::geo
