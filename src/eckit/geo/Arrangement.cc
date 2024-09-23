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


#include "eckit/geo/Arrangement.h"

#include "eckit/exception/Exceptions.h"


namespace eckit::geo {


Arrangement arrangement_from_string(const std::string& str) {
    return str == "F"   ? Arrangement::F
           : str == "T" ? Arrangement::T
           : str == "U" ? Arrangement::U
           : str == "V" ? Arrangement::V
           : str == "W" ? Arrangement::W
           : str == "C" ? Arrangement::C
           : str == "N" ? Arrangement::N
                        : throw AssertionFailed("Arrangement", Here());
}


std::string arrangement_to_string(Arrangement a) {
    return a == Arrangement::F   ? "F"
           : a == Arrangement::T ? "T"
           : a == Arrangement::U ? "U"
           : a == Arrangement::V ? "V"
           : a == Arrangement::W ? "W"
           : a == Arrangement::C ? "C"
           : a == Arrangement::N ? "N"
                                 : throw AssertionFailed("Arrangement", Here());
}


}  // namespace eckit::geo
