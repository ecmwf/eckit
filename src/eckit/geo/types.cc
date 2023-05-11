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


#include "eckit/geo/types.h"


#include "eckit/exception/Exceptions.h"


std::ostream& operator<<(std::ostream& out, const eckit::geo::Point& p) {
    return std::holds_alternative<eckit::geo::PointLonLat>(p)
               ? out << std::get<eckit::geo::PointLonLat>(p)
           : std::holds_alternative<eckit::geo::Point2>(p)
               ? out << std::get<eckit::geo::Point2>(p)
           : std::holds_alternative<eckit::geo::Point3>(p)
               ? out << std::get<eckit::geo::Point3>(p)
               : NOTIMP;
}


bool operator==(const eckit::geo::Point& p, const eckit::geo::Point& q) {
    ASSERT(p.index() == q.index());

    constexpr double eps = 1e-6;

    return std::holds_alternative<eckit::geo::PointLonLat>(p)
               ? points_equal(std::get<eckit::geo::PointLonLat>(p),
                              std::get<eckit::geo::PointLonLat>(q))
           : std::holds_alternative<eckit::geo::Point2>(p)
               ? points_equal(std::get<eckit::geo::Point2>(p), std::get<eckit::geo::Point2>(q))
           : std::holds_alternative<eckit::geo::Point3>(p)
               ? points_equal(std::get<eckit::geo::Point3>(p), std::get<eckit::geo::Point3>(q))
               : NOTIMP;
}
