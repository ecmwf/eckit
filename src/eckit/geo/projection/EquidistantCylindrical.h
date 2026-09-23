// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "eckit/geo/Projection.h"


namespace eckit::geo::projection {


/// Calculate coordinates of a point on an equidistant cylindrical (plate carrée) projection, given the standard
/// parallel the scale is true at, and the central parallel
class EquidistantCylindrical : public Projection {
public:

    // -- Constructors

    explicit EquidistantCylindrical(const Spec&);
    explicit EquidistantCylindrical(double lat_ts = 0, double lat_0 = 0, Figure* = nullptr);

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
