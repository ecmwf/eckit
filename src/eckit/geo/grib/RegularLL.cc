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


#include "eckit/geo/grib/RegularLL.h"


namespace eckit::geo::grib {


static const IteratorBuilder<RegularLL> __iterator("regular_ll");


RegularLL::RegularLL(const Configuration&) {
}


size_t RegularLL::size() const {
    return 0;
}


bool RegularLL::operator++() {
    return false;
}


}  // namespace eckit::geo::grib
