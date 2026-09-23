// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/log/IndentTarget.h"

#include <iostream>


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
