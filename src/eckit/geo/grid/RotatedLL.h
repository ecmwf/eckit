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

#include "eckit/geo/grid/LatLon.h"


namespace eckit::geo::grid {


class RotatedLL : public LatLon {
public:
    // -- Exceptions
    // None

    // -- Constructors

    RotatedLL(const Configuration&);
    RotatedLL(const Increments&, const Rotation&, const BoundingBox& = {},
              const PointLonLat& reference = {0, 0});

    // -- Destructor

    ~RotatedLL() override;

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

    Rotation rotation_;

    // -- Methods
    // None

    // -- Overridden methods
    Iterator* iterator() const override;
    void print(std::ostream&) const override;

    bool sameAs(const Grid&) const override;

    // From Grid
    const RotatedLL* croppedGrid(const BoundingBox&) const override;
    std::string factory() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geo::grid
