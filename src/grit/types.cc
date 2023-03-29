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

#include <cassert>


namespace grit {


PointLatLon::PointLatLon(double lat, double lon) : array{lat, lon} {
    assert(-90. <= lat && lat <= 90.);
}


}  // namespace grit
