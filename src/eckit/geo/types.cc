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

std::ostream &operator<<(std::ostream &out, const eckit::geo::Point &p)
{
    return std::holds_alternative<eckit::geo::PointLatLon>(p)
               ? out << std::get<eckit::geo::PointLatLon>(p)
           : std::holds_alternative<eckit::geo::PointXY>(p)
               ? out << std::get<eckit::geo::PointXY>(p)
           : std::holds_alternative<eckit::geo::PointXYZ>(p)
               ? out << std::get<eckit::geo::PointXYZ>(p)
               : NOTIMP;
}
