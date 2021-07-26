/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/exception/Exceptions.h"

#include "eckit/log/OStreamTarget.h"
#include "eckit/log/WrapperTarget.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

WrapperTarget::WrapperTarget(LogTarget* target) :
    target_(target), prefix_(true) {

    if (!target_) {
        target_ = new OStreamTarget(std::cout);
    }

    target_->attach();
}

WrapperTarget::~WrapperTarget() {
    target_->detach();
}

void WrapperTarget::write(const char* start, const char* end) {

    const char* begin = start;

    while (start != end) {
        if (*start == '\n') {
            target_->write(begin, start);
            writeSuffix();
            target_->write(start, start + 1);
            prefix_ = true;
            begin   = start + 1;
        }
        else {
            if (prefix_) {
                writePrefix();
                prefix_ = false;
            }
        }
        start++;
    }

    if (begin != end) {
        if (prefix_) {
            writePrefix();
            prefix_ = false;
        }
        target_->write(begin, end);
    }
}

void WrapperTarget::flush() {
    target_->flush();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
