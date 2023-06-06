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


namespace eckit::geo::grid {


class EquatorialAzimuthalEquidistant : public Grid {
public:
    // -- Exceptions
    // None

    // -- Constructors

    EquatorialAzimuthalEquidistant(const Configuration&);
    EquatorialAzimuthalEquidistant(const EquatorialAzimuthalEquidistant&) = delete;

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators

    EquatorialAzimuthalEquidistant& operator=(const EquatorialAzimuthalEquidistant&) = delete;

    // -- Methods
    // // None

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

    void print(std::ostream&) const override;

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
    // None
};


}  // namespace eckit::geo::grid
