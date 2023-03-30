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

#include "grit/Figure.h"


namespace grit::figure {


class Sphere final : public Figure {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    explicit Sphere(double R) : Figure(R) {}

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    // None

    // -- Overridden methods

    double angle(const PointLatLon&, const PointLatLon&) const override;
    double angle(const Point3&, const Point3&) const override;
    double distance(const PointLatLon&, const PointLatLon&) const override;
    double distance(const Point3&, const Point3&) const override;
    double area() const override;
    Point3 ll_to_xyz(const PointLatLon&, double height) const override;
    PointLatLon xyz_to_ll(const Point3&) const override;

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

}  // namespace grit::figure
