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


namespace eckit::geometry::grid {


class Gaussian : public Grid {
public:
    // -- Exceptions
    // None

    // -- Constructors

    Gaussian(size_t N, const area::BoundingBox& = {});
    Gaussian(const Configuration&);

    // -- Destructor

    ~Gaussian() override;

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
    // -- Members

    size_t N_;
    double angularPrecision_;
    mutable std::vector<double> latitudes_;

    // -- Methods

    const std::vector<double>& latitudes() const;

    bool angleApproximatelyEqual(const double&, const double&) const;

    void correctSouthNorth(double& s, double& n, bool in = true) const;

    // -- Overridden methods

    bool includesNorthPole() const override;
    bool includesSouthPole() const override;

    iterator begin() override { NOTIMP; }
    iterator end() override { NOTIMP; }

    const_iterator cbegin() const override { NOTIMP; }
    const_iterator cend() const override { NOTIMP; }

    const_iterator begin() const override { NOTIMP; }
    const_iterator end() const override { NOTIMP; }

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
    // None
};


}  // namespace eckit::geometry::grid
