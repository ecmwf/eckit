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


#include "eckit/geo/grid/regular/IrregularLL.h"

// #include <>


namespace eckit::geo::grid::regular {


IrregularLL::IrregularLL(const Spec& spec) :
    Regular(spec) {}


Grid::iterator IrregularLL::cbegin() const {
    NOTIMP;
}


Grid::iterator IrregularLL::cend() const {
    NOTIMP;
}


size_t IrregularLL::ni() const {
    NOTIMP;
}


size_t IrregularLL::nj() const {
    NOTIMP;
}


const std::vector<double>& IrregularLL::longitudes() const {
    NOTIMP;
}


const std::vector<double>& IrregularLL::latitudes() const {
    NOTIMP;
}


}  // namespace eckit::geo::grid::regular
