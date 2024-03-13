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

#include "eckit/geo/Grid.h"


namespace eckit::geo::iterator {
class Reduced;
}


namespace eckit::geo::grid {


class Reduced : public Grid {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors
    // None

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    // None

    // -- Overridden methods

    size_t size() const override;
    std::vector<Point> to_points() const override;
    std::pair<std::vector<double>, std::vector<double>> to_latlon() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
    // -- Constructors

    explicit Reduced(const Spec&);
    explicit Reduced(const area::BoundingBox&);

    // -- Members
    // None

    // -- Methods

    const std::vector<size_t>& niacc() const;

    virtual size_t ni(size_t j) const = 0;
    virtual size_t nj() const         = 0;

    virtual const std::vector<double>& latitudes() const   = 0;
    virtual std::vector<double> longitudes(size_t i) const = 0;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    mutable std::vector<size_t> niacc_;

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    friend class geo::iterator::Reduced;
};


}  // namespace eckit::geo::grid
