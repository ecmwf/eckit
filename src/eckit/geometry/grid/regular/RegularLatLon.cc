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


#include "eckit/geometry/grid/regular/RegularLatLon.h"

#include "eckit/geometry/iterator/Regular.h"


namespace eckit::geometry::grid::regular {


RegularLatLon::RegularLatLon(const Configuration& config) :
    Regular(config) {
}


Grid::iterator RegularLatLon::cbegin() const {
    return iterator{new geometry::iterator::Regular(*this, 0)};
}


Grid::iterator RegularLatLon::cend() const {
    return iterator{new geometry::iterator::Regular(*this, size())};
}


size_t RegularLatLon::ni() const {
    NOTIMP;
}


size_t RegularLatLon::nj() const {
    NOTIMP;
}


const std::vector<double>& RegularLatLon::longitudes() const {
    NOTIMP;
}


const std::vector<double>& RegularLatLon::latitudes() const {
    NOTIMP;
}


}  // namespace eckit::geometry::grid::regular
