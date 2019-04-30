/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <iostream>

#include "eckit/log/IndentTarget.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


IndentTarget::IndentTarget(const std::string& prefix, LogTarget* target, const char* space) :
    PrefixTarget("   " + prefix, target, space) {}

void IndentTarget::print(std::ostream& s) const {
    s << "IndentTarget(prefix=" << prefix_ << ", space=" << space_;
    if (target_) {
        s << ", target=" << *target_;
    }
    s << ")";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
