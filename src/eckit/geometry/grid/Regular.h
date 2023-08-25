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

#include "eckit/geometry/Grid.h"


namespace eckit::geometry::iterator {
class Regular;
}


namespace eckit::geometry::grid {


class Regular : public Grid {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors
    // None

    // -- Destructor

    ~Regular() override = default;

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    virtual size_t ni() const = 0;
    virtual size_t nj() const = 0;

    // -- Overridden methods

    size_t size() const override { return ni() * nj(); }

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
    // -- Constructors

    explicit Regular(const Configuration&);
    explicit Regular(const area::BoundingBox&);

    // -- Members
    // None

    // -- Methods

    virtual const std::vector<double>& longitudes() const = 0;
    virtual const std::vector<double>& latitudes() const  = 0;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    friend class ::eckit::geometry::iterator::Regular;
};


}  // namespace eckit::geometry::grid
