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

#include "eckit/geo/grid/Reduced.h"


namespace eckit::geo::grid::reduced {


class ReducedLL : public Reduced {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    explicit ReducedLL(const Configuration&);

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
    // None

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
