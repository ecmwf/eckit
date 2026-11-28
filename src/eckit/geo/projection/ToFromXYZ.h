// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <vector>

#include "eckit/geo/Figure.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"


namespace eckit::geo::projection {


// (lon, lat), at a height along the normal of a sphere or spheroid, to/from geocentric [x, y, z]; base of the
// projections doing so on their figure
class ToFromXYZ : public Projection {
public:

    // -- Class methods

    // geodetic (lon, lat) on a spheroid, a sphere if a ~= b
    static PointXYZ to_xyz(double a, double b, const PointLonLat&, double height = 0.);

    static PointXYZ to_xyz(double R, const PointLonLat& p, double height = 0.) { return to_xyz(R, R, p, height); }

    // direction of [x, y, z], at any distance from the centre
    static PointLonLat to_lonlat(const PointXYZ&);

    static PointLonLatH to_lonlath(double R, const PointXYZ&);

protected:

    // -- Constructors

    ToFromXYZ(Figure*, const Point& source);

    // -- Methods

    PointXYZ to_xyz(const PointLonLat& p, double height = 0.) const {
        return to_xyz(figure().a(), figure().b(), p, height);
    }

    // height can be empty
    std::vector<std::vector<double>> to_xyz(const std::vector<double>& lon, const std::vector<double>& lat,
                                            const std::vector<double>& height, double height_scale_factor) const;
};


}  // namespace eckit::geo::projection
