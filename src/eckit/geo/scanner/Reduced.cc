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


#include "grit/scanner/Reduced.h"

// #include <>


namespace grit::scanner {


Reduced::Reduced() = default;


bool Reduced::operator++() {
    return false;
}


size_t Reduced::size() const {
    return 0;
}


}  // namespace grit::scanner
