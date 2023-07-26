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

#include "eckit/geometry/grid/Gaussian.h"


namespace eckit {
class Fraction;
}


namespace eckit::geometry::grid {


class RegularGG final : public Gaussian {
public:
    // -- Exceptions
    // None

    // -- Constructors

    RegularGG(const Configuration&);
    RegularGG(size_t N, const area::BoundingBox& = {});

    // -- Destructor
    // None

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

private:
    // -- Members

    size_t k_;
    size_t Ni_;
    size_t Nj_;

    // -- Methods

    Fraction getSmallestIncrement() const;
    void correctWestEast(double& w, double& e) const;
    void setNiNj();

    // -- Overridden methods

    size_t size() const override;

    void print(std::ostream&) const override;
    bool isPeriodicWestEast() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geometry::grid
