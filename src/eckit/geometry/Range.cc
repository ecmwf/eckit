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


#include "eckit/geometry/Range.h"

#include "eckit/config/Configuration.h"
#include "eckit/exception/Exceptions.h"


namespace eckit::geometry {


Range::Range(const Configuration& config) :
    Range(config.getUnsigned("n")) {
}


Range::Range(size_t n) :
    n_(n) {
    ASSERT(n > 0);
}


}  // namespace eckit::geometry
