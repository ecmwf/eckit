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

#include "grit/exception.h"


std::ostream& operator<<(std::ostream& out, const grit::Point& p) {
    return std::holds_alternative<grit::PointLatLon>(p) ? out << std::get<grit::PointLatLon>(p)
           : std::holds_alternative<grit::PointXY>(p)   ? out << std::get<grit::PointXY>(p)
           : std::holds_alternative<grit::PointXYZ>(p)  ? out << std::get<grit::PointXYZ>(p)
                                                        : NOTIMP;
}
