// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/log/ColouringTarget.h"

#include <ostream>
#include <sstream>


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ColouringTarget::ColouringTarget(LogTarget* target, ColouringTarget::colourproc begin,
                                 ColouringTarget::colourproc end) :
    WrapperTarget(target) {
    std::ostringstream beginss;
    beginss << *begin;
    begin_ = beginss.str();

    std::ostringstream endss;
    endss << *end;
    end_ = endss.str();
}

ColouringTarget::~ColouringTarget() {}

void ColouringTarget::writePrefix() {
    target_->write(begin_.c_str(), begin_.c_str() + begin_.size());
}

void ColouringTarget::writeSuffix() {
    target_->write(end_.c_str(), end_.c_str() + end_.size());
}

void ColouringTarget::print(std::ostream& s) const {
    s << "ColouringTarget(";
    if (target_) {
        s << "target=" << *target_;
    }
    s << ")";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
