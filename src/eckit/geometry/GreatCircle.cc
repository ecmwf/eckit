/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/geometry/GreatCircle.h"

#include <cmath>
#include <limits>
#include <sstream>

#include "eckit/exception/Exceptions.h"
#include "eckit/types/FloatCompare.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit::geometry {

//----------------------------------------------------------------------------------------------------------------------

static double normalise_longitude(double a, const double& minimum) {
    while (a < minimum) {
        a += 360;
    }
    while (a >= minimum + 360) {
        a -= 360;
    }
    return a;
}

static const double radians_to_degrees = 180. * M_1_PI;

static const double degrees_to_radians = M_PI / 180.;

static constexpr std::streamsize max_digits10 = std::numeric_limits<double>::max_digits10;

static bool pole(const double lat) {
    return types::is_approximately_equal(std::abs(lat), 90.);
}

//----------------------------------------------------------------------------------------------------------------------

GreatCircle::GreatCircle(const PointLonLat& Alonlat, const PointLonLat& Blonlat) :
    A_(Alonlat), B_(Blonlat) {
    using namespace std;
    using types::is_approximately_equal;

    const bool Apole       = pole(A_.lat);
    const bool Bpole       = pole(B_.lat);
    const double lon12_deg = normalise_longitude(A_.lon - B_.lon, -180);

    const bool lon_same     = Apole || Bpole || is_approximately_equal(lon12_deg, 0.);
    const bool lon_opposite = Apole || Bpole || is_approximately_equal(abs(lon12_deg), 180.);
    const bool lat_same     = is_approximately_equal(A_.lat, B_.lat);
    const bool lat_opposite = is_approximately_equal(A_.lat, -B_.lat);

    if ((lat_same && lon_same) || (lat_opposite && lon_opposite)) {
        ostringstream oss;
        oss.precision(max_digits10);
        oss << "Great circle cannot be defined by points collinear with the centre, " << A_ << " and " << B_;
        throw BadValue(oss.str(), Here());
    }

    crossesPoles_ = lon_same || lon_opposite;
}

std::vector<double> GreatCircle::latitude(double lon) const {
    using namespace std;

    if (crossesPoles()) {
        return {};
    }

    const double lat1     = degrees_to_radians * A_.lat;
    const double lat2     = degrees_to_radians * B_.lat;
    const double lambda1p = degrees_to_radians * (lon - A_.lon);
    const double lambda2p = degrees_to_radians * (lon - B_.lon);
    const double lambda   = degrees_to_radians * normalise_longitude(B_.lon - A_.lon, -180);

    double lat = atan((tan(lat2) * sin(lambda1p) - tan(lat1) * sin(lambda2p)) / (sin(lambda)));
    return {radians_to_degrees * lat};
}

std::vector<double> GreatCircle::longitude(double lat) const {
    using namespace std;
    using types::is_approximately_equal;

    if (crossesPoles()) {
        const double lon = pole(A_.lat) ? B_.lon : A_.lon;
        return pole(lat) ? vector<double>{lon} : vector<double>{lon, lon + 180};
    }

    const double lon12 = degrees_to_radians * normalise_longitude(A_.lon - B_.lon, -180);
    const double lon1  = degrees_to_radians * A_.lon;
    const double lat1  = degrees_to_radians * A_.lat;
    const double lat2  = degrees_to_radians * B_.lat;
    const double lat3  = degrees_to_radians * lat;

    const double X = sin(lat1) * cos(lat2) * sin(lon12);
    const double Y = sin(lat1) * cos(lat2) * cos(lon12) - cos(lat1) * sin(lat2);

    if (is_approximately_equal(X, 0.) && is_approximately_equal(Y, 0.)) {
        return {};  // parallel (that is, equator)
    }

    const double lon0 = lon1 + atan2(Y, X);
    const double C    = cos(lat1) * cos(lat2) * tan(lat3) * sin(lon12) / sqrt(X * X + Y * Y);

    if (is_approximately_equal(C, -1.)) {
        return {radians_to_degrees * (lon0 + M_PI)};
    }

    if (is_approximately_equal(C, 1.)) {
        return {radians_to_degrees * lon0};
    }

    if (-1 < C && C < 1) {
        const double dlon = acos(C);
        return {radians_to_degrees * (lon0 - dlon + 2 * M_PI), radians_to_degrees * (lon0 + dlon)};
    }

    return {};
}

bool GreatCircle::crossesPoles() const {
    return crossesPoles_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::geometry
