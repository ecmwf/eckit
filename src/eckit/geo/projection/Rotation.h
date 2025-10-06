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
class Rotation : public Projection {
public:

    // -- Constructors

    explicit Rotation(const Spec&);
    explicit Rotation(const PointLonLat& = SOUTH_POLE, double angle = 0);

    // -- Methods

    PointLonLat south_pole() const { return south_pole_; }
    double angle() const { return angle_; }
    bool rotated() const { return rotated_; }

    inline PointLonLat fwd(const PointLonLat& p) const { return (*fwd_)(p); }
    inline PointLonLat inv(const PointLonLat& q) const { return (*inv_)(q); }

    // -- Overridden methods

    const std::string& type() const override;

    inline Point fwd(const Point& p) const override { return fwd(std::get<PointLonLat>(p)); }
    inline Point inv(const Point& q) const override { return inv(std::get<PointLonLat>(q)); }

    // -- Class methods

    [[nodiscard]] static Rotation* make_from_spec(const Spec&);

protected:

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;

private:

    // -- Types

    struct Implementation {
        Implementation()          = default;
        virtual ~Implementation() = default;

        Implementation(const Implementation&) = delete;
        Implementation(Implementation&&)      = delete;
        void operator=(const Implementation&) = delete;
        void operator=(Implementation&&)      = delete;

        virtual PointLonLat operator()(const PointLonLat&) const = 0;
    };

    // -- Members

    std::shared_ptr<Implementation> fwd_;
    std::shared_ptr<Implementation> inv_;

    PointLonLat south_pole_;
    double angle_;
    bool rotated_;
};


}  // namespace eckit::geo::projection
