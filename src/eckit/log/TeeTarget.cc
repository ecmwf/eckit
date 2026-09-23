// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/log/TeeTarget.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/log/OStreamTarget.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

TeeTarget::TeeTarget(LogTarget* left, LogTarget* right) : left_(left), right_(right) {

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
    if (start >= end)
        return;
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
