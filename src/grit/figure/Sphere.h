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

    explicit Sphere(double R);
    Sphere(const Sphere&) = default;
    Sphere(Sphere&&)      = default;

    // -- Destructor

    ~Sphere() override = default;

    // -- Convertors
    // None

    // -- Operators

    Sphere& operator=(const Sphere&) = default;
    Sphere& operator=(Sphere&&)      = default;

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
    // None

    // -- Methods
    // None

    // -- Overridden methods

    bool sphere() const override { return true; }
    double a() const override { return R_; }
    double b() const override { return R_; }
    double R() const override { return R_; }

    double angle(const PointLatLon&, const PointLatLon&) const override;
    double angle(const PointXYZ&, const PointXYZ&) const override;
    double distance(const PointLatLon&, const PointLatLon&) const override;
    double distance(const PointXYZ&, const PointXYZ&) const override;
    double area() const override;

    PointXYZ ll_to_xyz(const PointLatLon&) const override;
    PointLatLon xyz_to_ll(const PointXYZ&) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    double R_;

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
