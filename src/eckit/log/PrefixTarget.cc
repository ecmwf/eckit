/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cstring>
#include <iostream>

#include "eckit/log/PrefixTarget.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


PrefixTarget::PrefixTarget(const std::string& prefix, LogTarget* target, const char* space) :
    WrapperTarget(target),
    prefix_(prefix),
    space_(space),
    prefixLength_(prefix.size()),
    spaceLength_(::strlen(space)) {}

void PrefixTarget::writePrefix() {
    const char* p = prefix_.c_str();
    target_->write(p, p + prefixLength_);
    target_->write(space_, space_ + spaceLength_);
}


void PrefixTarget::writeSuffix() {}

void PrefixTarget::print(std::ostream& s) const {
    s << "PrefixTarget(prefix=" << prefix_ << ", space=" << space_ << ")";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
