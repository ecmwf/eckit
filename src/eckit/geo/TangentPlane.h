// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/PointXY.h"
#include "eckit/geo/PointXYZ.h"


namespace eckit::geo {


class Figure;


/**
 * @brief Local tangent plane at a point, with axes east and north
 *
 * Offsets of nearby points are their projection on the plane, in the units of the coordinates, valid for distances much
 * smaller than the figure. From a position alone, the plane is normal to the radial direction (sphere); from a figure
 * and a geodetic longitude/latitude, it is normal to the surface (sphere or oblate spheroid), with the origin on the
 * surface. At the poles, east is the Y axis.
 */
class TangentPlane {
public:

    // -- Constructors

    explicit TangentPlane(const PointXYZ& origin);

    /// Tangent plane at a geodetic longitude/latitude [degree] on the surface of a figure: the origin is computed from
    /// both, so it is always consistent with the axes
    TangentPlane(const Figure&, const PointLonLat& geodetic);

    // -- Operators

    /// Offset (east, north) of a point from the tangent point
    PointXY operator()(const PointXYZ&) const;

    // -- Methods

    const PointXYZ& origin() const { return origin_; }
    const PointXYZ& east() const { return east_; }
    const PointXYZ& north() const { return north_; }

private:

    // -- Members

    PointXYZ origin_;
    PointXYZ east_;
    PointXYZ north_;
};


}  // namespace eckit::geo
