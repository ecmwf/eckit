// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
