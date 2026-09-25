// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <string>

#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"


namespace eckit::geo::projection {


/**
 * @brief SpaceView projection
 * @ref LRIT/HRIT Global Specification (CGMS 03, Issue 2.6, 12.08.1999)
 */
class SpaceView : public Projection {
public:

    // -- Constructors

    explicit SpaceView(const Spec&);

    // -- Methods

    PointXY fwd(const PointLonLat&) const;
    PointLonLat inv(const PointXY&) const;

    // -- Overridden methods

    const std::string& type() const override;

    inline Point fwd(const Point& p) const override { return fwd(std::get<PointLonLat>(p)); }
    inline Point inv(const Point& q) const override { return inv(std::get<PointXY>(q)); }

protected:

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;
};


}  // namespace eckit::geo::projection
