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

#include <array>

#include "grit/Projection.h"


namespace grit::projection {


/// Compute coordinates of a point on a rotated sphere given new location of South Pole (vector) and angle
class Rotation final : public Projection {
public:
    // -- Types

    struct MatrixXYZ : std::array<double, 9> {
        MatrixXYZ() : array{1, 0, 0, 0, 1, 0, 0, 0, 1} {}

        MatrixXYZ(double xx, double xy, double xz, double yx, double yy, double yz, double zx, double zy, double zz) :
            array{xx, xy, xz, yx, yy, yz, zx, zy, zz} {}

        MatrixXYZ(const MatrixXYZ& other) : array(other) {}
        MatrixXYZ(MatrixXYZ&& other) : array(other) {}

        ~MatrixXYZ() = default;

        MatrixXYZ& operator=(const MatrixXYZ& other) {
            array::operator=(other);
            return *this;
        }
        MatrixXYZ& operator=(MatrixXYZ&& other) {
            array::operator=(other);
            return *this;
        }

        double& xx = operator[](0);
        double& xy = operator[](1);
        double& xz = operator[](2);
        double& yx = operator[](3);
        double& yy = operator[](4);
        double& yz = operator[](5);
        double& zx = operator[](6);
        double& zy = operator[](7);
        double& zz = operator[](8);

        PointXYZ operator*(const PointXYZ& p) const {
            return {xx * p.x + xy * p.y + xz * p.z, yx * p.x + yy * p.y + yz * p.z, zx * p.x + zy * p.y + zz * p.z};
        }
    };

    // -- Exceptions
    // None

    // -- Constructors

    Rotation(PointLatLon south_pole, double angle);

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
    // -- Types

    enum class rotation_type
    {
        UNROTATED,
        ANGLE,
        ANGLE_VECTOR
    };

    // -- Members

    rotation_type rotation_ = rotation_type::UNROTATED;

    MatrixXYZ R_;
    MatrixXYZ U_;
    double angle_ = 0.;

    // -- Methods
    // None

    // -- Overridden methods

    Point direct(const Point&) const override;
    Point inverse(const Point&) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace grit::projection
