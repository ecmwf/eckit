/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/log/PrefixTarget.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------


PrefixTarget::PrefixTarget(const std::string& prefix, LogTarget* target):
    WrapperTarget(target),
    prefix_(prefix + " ")
{
}

void PrefixTarget::writePrefix() {
    const char* p = prefix_.c_str();
    target_->write(p, p + prefix_.size());
}


void PrefixTarget::writeSuffix() {
}

//-----------------------------------------------------------------------------

} // namespace eckit
