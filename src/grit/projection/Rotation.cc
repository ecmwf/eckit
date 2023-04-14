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


#include "grit/projection/Rotation.h"

#include "grit/exception.h"


namespace grit::projection {


Rotation::Rotation() = default;


Point grit::projection::Rotation::direct(const Point& p) const {
    ASSERT(std::holds_alternative<PointLatLon>(p));

    NOTIMP;
}


Point grit::projection::Rotation::inverse(const Point& p) const {
    ASSERT(std::holds_alternative<PointLatLon>(p));

    NOTIMP;
}


}  // namespace grit::projection
