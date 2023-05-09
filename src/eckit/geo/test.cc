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


#include "grit/test.h"


bool operator==(const grit::Point& p, const grit::Point& q) {
    ASSERT(p.index() == q.index());

    constexpr double eps = 1e-6;

    return std::holds_alternative<grit::PointLatLon>(p)
               ? std::get<grit::PointLatLon>(p).is_approximately_equal(std::get<grit::PointLatLon>(q), eps)
           : std::holds_alternative<grit::PointXY>(p)
               ? std::get<grit::PointXY>(p).is_approximately_equal(std::get<grit::PointXY>(q), eps)
           : std::holds_alternative<grit::PointXYZ>(p)
               ? std::get<grit::PointXYZ>(p).is_approximately_equal(std::get<grit::PointXYZ>(q), eps)
               : NOTIMP;
}
