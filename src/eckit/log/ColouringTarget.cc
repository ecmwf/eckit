/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <ostream>

#include "eckit/log/ColouringTarget.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ColouringTarget::ColouringTarget(LogTarget* target, ColouringTarget::colourproc begin, ColouringTarget::colourproc end) {

    std::ostringstream beginss;
    beginss << *begin;
    begin_ = beginss.str();

    std::ostringstream endss;
    endss << *end;
    end_ = endss.str();
}

void ColouringTarget::writePrefix() {
    target_->write(begin_.c_str(), begin_.c_str() + begin_.size());
}

void ColouringTarget::writeSuffix() {
    target_->write(end_.c_str(), end_.c_str() + end_.size());
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
