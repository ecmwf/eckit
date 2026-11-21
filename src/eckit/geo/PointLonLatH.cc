// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/PointLonLatH.h"

#include "eckit/types/FloatCompare.h"


namespace eckit::geo {


PointLonLatH PointLonLatH::make(value_type lon, value_type lat, value_type height, value_type lon_minimum,
                                value_type eps) {
    return {PointLonLat::make(lon, lat, lon_minimum, eps), height};
}


bool points_equal(const PointLonLatH& a, const PointLonLatH& b, PointLonLatH::value_type eps) {
    return points_equal(a.lonlat(), b.lonlat(), eps) && types::is_approximately_equal(a.height(), b.height(), eps);
}


}  // namespace eckit::geo
