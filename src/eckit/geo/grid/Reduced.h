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


class Reduced : public Gaussian {
public:
    // -- Exceptions
    // None

    // -- Constructors

    Reduced(const Configuration&);
    Reduced(size_t N, const std::vector<long>& pl, const BoundingBox& = BoundingBox(),
            double angularPrecision = 0);

    // -- Destructor

    ~Reduced() override = default;

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    static std::vector<long> pls(const std::string&);

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
    // -- Constructors

    Reduced(size_t N, const BoundingBox& = BoundingBox(), double angularPrecision = 0);

    // -- Members

    size_t k_;
    size_t Nj_;

    // -- Methods

    const std::vector<long>& pls() const;

    void setNj(std::vector<long>, double s, double n);
    void correctWestEast(double& w, double& e) const;

    // -- Overridden methods

    bool isPeriodicWestEast() const override;

    // -- Class members

    // -- Class methods
    // None

private:
    // -- Members

    std::vector<long> pl_;

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
