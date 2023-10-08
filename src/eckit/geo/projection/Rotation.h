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

#include "eckit/geo/Projection.h"


namespace eckit::geo::projection {


/// Calculate coordinates of a point on a rotated sphere given new location of South Pole (vector) and angle
class Rotation final : public Projection {
public:
    // -- Exceptions
    // None

    // -- Constructors

    Rotation();
    Rotation(double south_pole_lon, double south_pole_lat, double angle);
    explicit Rotation(const Configuration&);

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    bool rotated() const { return rotated_; }
    PointLonLat fwd(const PointLonLat& p) const { return (*fwd_)(p); }
    PointLonLat inv(const PointLonLat& q) const { return (*inv_)(q); }

    // -- Overridden methods

    Spec spec() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Types

    struct Rotate {
        Rotate()          = default;
        virtual ~Rotate() = default;

        Rotate(const Rotate&)         = delete;
        Rotate(Rotate&&)              = delete;
        void operator=(const Rotate&) = delete;
        void operator=(Rotate&&)      = delete;

        virtual PointLonLat operator()(const PointLonLat&) const = 0;
        virtual Spec spec() const                                = 0;
    };

    // -- Members

    std::unique_ptr<Rotate> fwd_;
    std::unique_ptr<Rotate> inv_;
    bool rotated_;

    // -- Methods
    // None

    // -- Overridden methods

    Point fwd(const Point& p) const override { return (*fwd_)(std::get<PointLonLat>(p)); }
    Point inv(const Point& q) const override { return (*inv_)(std::get<PointLonLat>(q)); }

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geo::projection
