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


#include "eckit/geo/test.h"

#include "eckit/exception/Exceptions.h"

bool operator==(const eckit::geo::Point& p, const eckit::geo::Point& q) {
    ASSERT(p.index() == q.index());

    constexpr double eps = 1e-6;

    return std::holds_alternative<eckit::geo::PointLatLon>(p)
               ? std::get<eckit::geo::PointLatLon>(p)
                     .is_approximately_equal(std::get<eckit::geo::PointLatLon>(q), eps)
           : std::holds_alternative<eckit::geo::PointXY>(p)
               ? std::get<eckit::geo::PointXY>(p)
                     .is_approximately_equal(std::get<eckit::geo::PointXY>(q), eps)
           : std::holds_alternative<eckit::geo::PointXYZ>(p)
               ? std::get<eckit::geo::PointXYZ>(p)
                     .is_approximately_equal(std::get<eckit::geo::PointXYZ>(q), eps)
               : NOTIMP;
}
