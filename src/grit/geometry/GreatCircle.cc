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


#include "grit/geometry/GreatCircle.h"

#include <cassert>
#include <cmath>
#include <limits>

#include "grit/util.h"


namespace grit::geometry {


static bool pole(double lat) {
    return util::approximately_equal(std::abs(lat), 90.);
}


GreatCircle::GreatCircle(const Point2& Alonlat, const Point2& Blonlat) : A_(Alonlat), B_(Blonlat) {
    const bool Apole       = pole(A_[1]);
    const bool Bpole       = pole(B_[1]);
    const double lon12_deg = util::normalise_longitude_to_minimum(A_[0] - B_[0], -180.);

    const bool lon_same     = Apole || Bpole || util::approximately_equal(lon12_deg, 0.);
    const bool lon_opposite = Apole || Bpole || util::approximately_equal(std::abs(lon12_deg), 180.);
    const bool lat_same     = util::approximately_equal(A_[1], B_[1]);
    const bool lat_opposite = util::approximately_equal(A_[1], -B_[1]);

    assert(!(lat_same && lon_same) && !(lat_opposite && lon_opposite) &&
           "Great circle cannot be defined by points collinear with the centre");

    crossesPoles_ = lon_same || lon_opposite;
}


std::vector<double> GreatCircle::latitude(double lon) const {
    if (crossesPoles()) {
        return {};
    }

    const double lat1     = util::degrees_to_radians * A_[1];
    const double lat2     = util::degrees_to_radians * B_[1];
    const double lambda1p = util::degrees_to_radians * (lon - A_[0]);
    const double lambda2p = util::degrees_to_radians * (lon - B_[0]);
    const double lambda   = util::degrees_to_radians * util::normalise_longitude_to_minimum(B_[0] - A_[0], -180.);

    double lat =
        std::atan((std::tan(lat2) * std::sin(lambda1p) - std::tan(lat1) * std::sin(lambda2p)) / (std::sin(lambda)));
    return {util::radians_to_degrees * lat};
}


std::vector<double> GreatCircle::longitude(double lat) const {
    if (crossesPoles()) {
        const double lon = pole(A_[1]) ? B_[0] : A_[0];
        if (pole(lat)) {
            return {lon};
        }

        return {lon, lon + 180.};
    }

    const double lon12 = util::degrees_to_radians * util::normalise_longitude_to_minimum(A_[0] - B_[0], -180.);
    const double lon1  = util::degrees_to_radians * A_[0];
    const double lat1  = util::degrees_to_radians * A_[1];
    const double lat2  = util::degrees_to_radians * B_[1];
    const double lat3  = util::degrees_to_radians * lat;

    const double X = std::sin(lat1) * std::cos(lat2) * std::sin(lon12);
    const double Y = std::sin(lat1) * std::cos(lat2) * std::cos(lon12) - std::cos(lat1) * std::sin(lat2);

    if (util::approximately_equal(X, 0.) && util::approximately_equal(Y, 0.)) {
        return {};  // parallel (that is, equator)
    }

    const double lon0 = lon1 + std::atan2(Y, X);
    const double C    = std::cos(lat1) * std::cos(lat2) * std::tan(lat3) * std::sin(lon12) / std::sqrt(X * X + Y * Y);

    if (util::approximately_equal(C, -1.)) {
        return {util::radians_to_degrees * (lon0 + M_PI)};
    }

    if (util::approximately_equal(C, 1.)) {
        return {util::radians_to_degrees * lon0};
    }

    if (-1 < C && C < 1) {
        const double dlon = std::acos(C);
        return {util::radians_to_degrees * (lon0 - dlon + 2 * M_PI), util::radians_to_degrees * (lon0 + dlon)};
    }

    return {};
}


}  // namespace grit::geometry
