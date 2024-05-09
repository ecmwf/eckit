/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/geo/GreatCircle.h"

#include <cmath>
#include <limits>
#include <sstream>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/util.h"
#include "eckit/types/FloatCompare.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit::geo {

//----------------------------------------------------------------------------------------------------------------------

using types::is_approximately_equal;

static bool pole(const double lat) {
    return is_approximately_equal(std::abs(lat), 90.);
}

//----------------------------------------------------------------------------------------------------------------------

GreatCircle::GreatCircle(const PointLonLat& Alonlat, const PointLonLat& Blonlat) : A_(Alonlat), B_(Blonlat) {
    const bool Apole       = pole(A_.lat);
    const bool Bpole       = pole(B_.lat);
    const double lon12_deg = PointLonLat::normalise_angle_to_minimum(A_.lon - B_.lon, -180.);

    const bool lon_same     = Apole || Bpole || is_approximately_equal(lon12_deg, 0.);
    const bool lon_opposite = Apole || Bpole || is_approximately_equal(std::abs(lon12_deg), 180.);
    const bool lat_same     = is_approximately_equal(A_.lat, B_.lat);
    const bool lat_opposite = is_approximately_equal(A_.lat, -B_.lat);

    if ((lat_same && lon_same) || (lat_opposite && lon_opposite)) {
        std::ostringstream oss;
        oss.precision(std::numeric_limits<double>::max_digits10);
        oss << "Great circle cannot be defined by points collinear with the centre, " << A_ << " and " << B_;
        throw BadValue(oss.str(), Here());
    }

    crossesPoles_ = lon_same || lon_opposite;
}

std::vector<double> GreatCircle::latitude(double lon) const {
    if (crossesPoles()) {
        return {};
    }

    const double lat1     = util::DEGREE_TO_RADIAN * A_.lat;
    const double lat2     = util::DEGREE_TO_RADIAN * B_.lat;
    const double lambda1p = util::DEGREE_TO_RADIAN * (lon - A_.lon);
    const double lambda2p = util::DEGREE_TO_RADIAN * (lon - B_.lon);
    const double lambda   = util::DEGREE_TO_RADIAN * PointLonLat::normalise_angle_to_minimum(B_.lon - A_.lon, -180.);

    double lat
        = std::atan((std::tan(lat2) * std::sin(lambda1p) - std::tan(lat1) * std::sin(lambda2p)) / (std::sin(lambda)));
    return {util::RADIAN_TO_DEGREE * lat};
}

std::vector<double> GreatCircle::longitude(double lat) const {
    if (crossesPoles()) {
        const double lon = pole(A_.lat) ? B_.lon : A_.lon;
        if (pole(lat)) {
            return {lon};
        }

        return {lon, lon + 180.};
    }

    const double lon12 = util::DEGREE_TO_RADIAN * PointLonLat::normalise_angle_to_minimum(A_.lon - B_.lon, -180.);
    const double lon1  = util::DEGREE_TO_RADIAN * A_.lon;
    const double lat1  = util::DEGREE_TO_RADIAN * A_.lat;
    const double lat2  = util::DEGREE_TO_RADIAN * B_.lat;
    const double lat3  = util::DEGREE_TO_RADIAN * lat;

    const double X = std::sin(lat1) * std::cos(lat2) * std::sin(lon12);
    const double Y = std::sin(lat1) * std::cos(lat2) * std::cos(lon12) - std::cos(lat1) * std::sin(lat2);

    if (is_approximately_equal(X, 0.) && is_approximately_equal(Y, 0.)) {
        return {};  // parallel (that is, equator)
    }

    const double lon0 = lon1 + atan2(Y, X);
    const double C    = std::cos(lat1) * std::cos(lat2) * std::tan(lat3) * std::sin(lon12) / std::sqrt(X * X + Y * Y);

    if (is_approximately_equal(C, -1.)) {
        return {util::RADIAN_TO_DEGREE * (lon0 + M_PI)};
    }

    if (is_approximately_equal(C, 1.)) {
        return {util::RADIAN_TO_DEGREE * lon0};
    }

    if (-1 < C && C < 1) {
        const double dlon = std::acos(C);
        return {util::RADIAN_TO_DEGREE * (lon0 - dlon + 2 * M_PI), util::RADIAN_TO_DEGREE * (lon0 + dlon)};
    }

    return {};
}

bool GreatCircle::crossesPoles() const {
    return crossesPoles_;
}

std::pair<double, double> GreatCircle::calculate_course(const PointLonLat& A, const PointLonLat& B) {
    const auto sdl = std::sin(util::DEGREE_TO_RADIAN * (B.lon - A.lon));
    const auto cdl = std::cos(util::DEGREE_TO_RADIAN * (B.lon - A.lon));
    const auto spA = std::sin(util::DEGREE_TO_RADIAN * A.lat);
    const auto cpA = std::cos(util::DEGREE_TO_RADIAN * A.lat);
    const auto spB = std::sin(util::DEGREE_TO_RADIAN * B.lat);
    const auto cpB = std::cos(util::DEGREE_TO_RADIAN * B.lat);

    const auto alpha1 = util::RADIAN_TO_DEGREE * std::atan2(cpB * sdl, cpA * spB - spA * cpB * cdl);
    const auto alpha2 = util::RADIAN_TO_DEGREE * std::atan2(cpA * sdl, -cpB * spA + spB * cpA * cdl);

    return {alpha1, alpha2};
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::geo
