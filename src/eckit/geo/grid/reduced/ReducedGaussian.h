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

#include <memory>

#include "eckit/geo/grid/Reduced.h"
#include "eckit/geo/util.h"


namespace eckit::geo {
class Range;}



namespace eckit::geo::grid::reduced {


class ReducedGaussian : public Reduced {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    explicit ReducedGaussian(const Configuration&);
    ReducedGaussian(size_t N, const pl_type& pl, const area::BoundingBox& = area::BoundingBox::make_global_prime());

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    // None

    // -- Overridden methods

    iterator cbegin() const override;
    iterator cend() const override;

    size_t ni(size_t j) const override;
    size_t nj() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    const pl_type pl_;
    size_t j_;
    size_t Nj_;

    std::unique_ptr<Range> x_;
    std::unique_ptr<Range> y_;

    // -- Methods
    // None

    // -- Overridden methods

    const std::vector<double>& latitudes() const override;
    std::vector<double> longitudes(size_t j) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geo::grid::reduced
