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


#include "eckit/geometry/grid/IrregularLatlon.h"

#include <cmath>

#include "eckit/utils/MD5.h"

#include "eckit/geometry/Projection.h"


namespace eckit::geometry::grid {


static void range(const std::vector<double>& v, double& mn, double& mx, double& dmax) {
    ASSERT(v.size() >= 2);

    dmax = 0;
    mx   = v[0];
    mn   = v[0];

    for (size_t i = 1; i < v.size(); ++i) {
        double d = std::abs(v[i] - v[i - 1]);
        dmax     = std::max(d, dmax);
        mx       = std::max(v[i], mx);
        mn       = std::min(v[i], mn);
    }
}


IrregularLatlon::IrregularLatlon(const Configuration& config) :
    Grid(config) {
    ASSERT(config.get("latitudes", latitudes_));
    range(latitudes_, south_, north_, south_north_);

    ASSERT(config.get("longitudes", longitudes_));
    range(longitudes_, west_, east_, west_east_);
}


size_t IrregularLatlon::size() const {
    return latitudes_.size() * longitudes_.size();
}


bool IrregularLatlon::isPeriodicWestEast() const {
    return (east_ - west_) + west_east_ >= GLOBE;
}


bool IrregularLatlon::includesNorthPole() const {
    return north_ + south_north_ >= NORTH_POLE;
}


bool IrregularLatlon::includesSouthPole() const {
    return south_ - south_north_ <= SOUTH_POLE;
}


// static const GridRegisterType<IrregularLatlon> irregularLatlon("irregular_latlon");


}  // namespace eckit::geometry::grid
