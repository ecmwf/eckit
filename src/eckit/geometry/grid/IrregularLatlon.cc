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

#include <algorithm>

#include "eckit/config/Configuration.h"
#include "eckit/geometry/area/BoundingBox.h"


namespace eckit::geometry::grid {


static void range(const std::vector<double>& v, double& mn, double& mx) {
    ASSERT(!v.empty());

    mx = v[0];
    mn = v[0];
    for (size_t i = 1; i < v.size(); ++i) {
        mx = std::max(v[i], mx);
        mn = std::min(v[i], mn);
    }
}


IrregularLatlon::IrregularLatlon(const Configuration& config) :
    Grid(config) {
    double south = 0;
    double north = 0;
    ASSERT(config.get("latitudes", latitudes_));
    range(latitudes_, south, north);

    double west = 0;
    double east = 0;
    ASSERT(config.get("longitudes", longitudes_));
    range(longitudes_, west, east);

    Grid::boundingBox({north, west, south, east});
}


Grid::iterator IrregularLatlon::cbegin() const {
    NOTIMP;
}


Grid::iterator IrregularLatlon::cend() const {
    NOTIMP;
}


size_t IrregularLatlon::size() const {
    return latitudes_.size() * longitudes_.size();
}


static const GridRegisterType<IrregularLatlon> irregularLatlon("irregular_latlon");


}  // namespace eckit::geometry::grid
