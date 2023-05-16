/*
 * (C) Copyright 2023 UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef CoordinateHelpers_H
#define CoordinateHelpers_H

namespace eckit::geometry {

class Point2;

//----------------------------------------------------------------------------------------------------------------------

/// Shift angle in increments of 360° until it lies in [minimum, minimum+360°).
///
/// Inputs angle and minimum are in degrees, returned angle is in degrees.
double normalise_angle(double angle, double minimum);

//----------------------------------------------------------------------------------------------------------------------

/// Shift input point on sphere so its longitude lies in [minimum_lon, minimum_lon+360°)
/// and its latitude lies in [-90°, 90°].
///
/// Latitudes outside the canonical interval [-90°,90°] are first shifted into the interval
/// [-90°,270°], then any points with latitudes in [90°,270°] are flagged as "across the pole".
/// Such points are re-labeled with equivalent coordinates that lie within the canonical coordinate
/// patch by the transformation: (λ, ϕ) -> (λ+180°, 180°-ϕ).
///
/// Finally, the longitude is shifted into [minimum_lon, minimum_lon+360°).
///
/// Inputs lonlat and minimum_lon are in degrees, returned angles are in degrees.
Point2 canonicaliseOnSphere(const Point2& lonlat, double minimum_lon = 0.);

//----------------------------------------------------------------------------------------------------------------------

/// Assert latitude lies in [-90°, 90°].
void assert_latitude_range(double lat);


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::geometry

#endif  // CoordinateHelpers_H
