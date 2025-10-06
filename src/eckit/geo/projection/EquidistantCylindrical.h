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
class EquidistantCylindrical : public Projection {
public:

    // -- Constructors

    explicit EquidistantCylindrical(const Spec&);
    explicit EquidistantCylindrical(double lat_ts = 0, double lat_0 = 0);

    // -- Methods

    inline PointXY fwd(const PointLonLat& p) const { return impl_->fwd(p); }
    inline PointLonLat inv(const PointXY& q) const { return impl_->inv(q); }

    // -- Overridden methods

    const std::string& type() const override;

    inline Point fwd(const Point& p) const override { return fwd(std::get<PointLonLat>(p)); }
    inline Point inv(const Point& q) const override { return inv(std::get<PointXY>(q)); }

    // -- Class methods

    [[nodiscard]] static EquidistantCylindrical* make_from_spec(const Spec&);

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

        virtual PointXY fwd(const PointLonLat&) const = 0;
        virtual PointLonLat inv(const PointXY&) const = 0;
    };

    // -- Members

    std::shared_ptr<Implementation> impl_;

    double lat_ts_;
    double lat_0_;
};


}  // namespace eckit::geo::projection
