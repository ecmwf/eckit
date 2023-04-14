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


#include "grit/types.h"

#include <iostream>

#include "grit/exception.h"


namespace grit {


PointLatLon::PointLatLon(double lat, double lon) : array{lat, lon} {
    ASSERT_MSG(-90. <= lat && lat <= 90., "PointLatLon: invalid latitude");
}


std::ostream& operator<<(std::ostream& out, const Point& P) {
    if (std::holds_alternative<PointLatLon>(P)) {
        const auto& Q = std::get<PointLatLon>(P);
        return out << '{' << Q.lat << ", " << Q.lon << '}';
    }

    if (std::holds_alternative<PointXY>(P)) {
        const auto& Q = std::get<PointXY>(P);
        return out << '{' << Q.x << ", " << Q.y << '}';
    }

    if (std::holds_alternative<PointXYZ>(P)) {
        const auto& Q = std::get<PointXYZ>(P);
        return out << '{' << Q.x << ", " << Q.y << ", " << Q.z << '}';
    }

    NOTIMP;
}


}  // namespace grit
