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


#include "eckit/geo/concept/Range.h"

// #include <>


namespace eckit::geo::concept {


    Range::Range(size_t N, double a, double b, double inc, double ref) :
        N_(N) {
    }


}  // namespace eckit::geo::concept
