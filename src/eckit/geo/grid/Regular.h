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

#include "eckit/geo/grid/Gaussian.h"


namespace eckit::geo::grid {


class Regular : public Gaussian {
public:
    // -- Exceptions
    // None

    // -- Constructors

    Regular(const Configuration&);
    Regular(size_t N, const BoundingBox& = BoundingBox(), double angularPrecision = 0);

    // -- Destructor

    virtual ~Regular() override = default;

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
    // -- Constructors
    // None

    // -- Members

    size_t k_;
    size_t Ni_;
    size_t Nj_;

    // -- Methods

    void correctWestEast(double& w, double& e) const;
    void setNiNj();

    // -- Overridden methods

    bool sameAs(const Grid&) const override;

    // from Grid
    bool isPeriodicWestEast() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    Regular();

    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods

    size_t numberOfPoints() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geo::grid
