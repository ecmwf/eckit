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


#include "eckit/geometry/grid/regular/IrregularLatLon.h"

// #include <>


namespace eckit::geometry::grid::regular {


IrregularLatLon::IrregularLatLon(const Configuration& config) :
    Regular(config) {
}


Grid::iterator IrregularLatLon::cbegin() const {
    NOTIMP;
}


Grid::iterator IrregularLatLon::cend() const {
    NOTIMP;
}


size_t IrregularLatLon::ni() const {
    NOTIMP;
}


size_t IrregularLatLon::nj() const {
    NOTIMP;
}


const std::vector<double>& IrregularLatLon::longitudes() const {
    NOTIMP;
}


const std::vector<double>& IrregularLatLon::latitudes() const {
    NOTIMP;
}


}  // namespace eckit::geometry::grid::regular
