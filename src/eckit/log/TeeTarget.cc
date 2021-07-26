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
#include "eckit/log/TeeTarget.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

TeeTarget::TeeTarget(LogTarget* left, LogTarget* right) :
    left_(left), right_(right) {

    if (left_) {
        left_->attach();
    }

    if (right_) {
        right_->attach();
    }
}

TeeTarget::~TeeTarget() {
    if (left_) {
        left_->detach();
    }

    if (right_) {
        right_->detach();
    }
}

void TeeTarget::write(const char* start, const char* end) {
    if (left_) {
        left_->write(start, end);
    }

    if (right_) {
        right_->write(start, end);
    }
}

void TeeTarget::flush() {
    if (left_) {
        left_->flush();
    }

    if (right_) {
        right_->flush();
    }
}

void TeeTarget::print(std::ostream& s) const {
    s << "TeeTarget(";
    if (left_) {
        s << *left_;
    }
    if (right_) {
        s << *right_;
    }
    s << ")";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
