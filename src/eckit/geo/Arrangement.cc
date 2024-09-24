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
    return str == "F"   ? Arrangement::ORCA_F
           : str == "T" ? Arrangement::ORCA_T
           : str == "U" ? Arrangement::ORCA_U
           : str == "V" ? Arrangement::ORCA_V
           : str == "W" ? Arrangement::ORCA_W
           : str == "C" ? Arrangement::FESOM_C
           : str == "N" ? Arrangement::FESOM_N
                        : throw AssertionFailed("Arrangement", Here());
}


std::string arrangement_to_string(Arrangement a) {
    return a == Arrangement::ORCA_F    ? "F"
           : a == Arrangement::ORCA_T  ? "T"
           : a == Arrangement::ORCA_U  ? "U"
           : a == Arrangement::ORCA_V  ? "V"
           : a == Arrangement::ORCA_W  ? "W"
           : a == Arrangement::FESOM_C ? "C"
           : a == Arrangement::FESOM_N ? "N"
                                       : throw AssertionFailed("Arrangement", Here());
}


}  // namespace eckit::geo
