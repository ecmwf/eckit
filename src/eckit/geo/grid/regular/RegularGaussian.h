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

#include "eckit/geo/Range.h"
#include "eckit/geo/grid/Regular.h"


namespace eckit::geo::grid::regular {


class RegularGaussian final : public Regular {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    explicit RegularGaussian(const Spec&);
    explicit RegularGaussian(size_t N, const area::BoundingBox& = area::BoundingBox::make_global_prime());

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

    size_t ni() const override { return x_->size(); }
    size_t nj() const override { return y_->size(); }

    // -- Class members
    // None

    // -- Class methods

    static Spec* spec(const std::string& name);

private:
    // -- Members

    const size_t N_;

    std::unique_ptr<Range> x_;
    std::unique_ptr<Range> y_;

    // -- Methods
    // None

    // -- Overridden methods

    const std::vector<double>& longitudes() const override;
    const std::vector<double>& latitudes() const override;

    void spec(spec::Custom&) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geo::grid::regular
