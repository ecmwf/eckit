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
#include "eckit/maths/Functions.h"

#include "eckit/log/LineBasedTarget.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

LineBasedTarget::LineBasedTarget() :
    size_(4096), buffer_(new char[size_]), position_(0) {
    ASSERT(buffer_);
}

LineBasedTarget::~LineBasedTarget() {
    delete[] buffer_;
}

void LineBasedTarget::reserve(size_t size) {
    if (size_ < size) {
        delete[] buffer_;
        size_   = round(size, 1024 * 1024);
        buffer_ = new char[size_];
        ASSERT(buffer_);
    }
}

void LineBasedTarget::write(const char* start, const char* end) {

    reserve(position_ + (end - start) + 1);

    while (start != end) {
        if (*start == '\n') {
            buffer_[position_] = 0;
            line(buffer_);
            position_ = 0;
            start++;
        }
        else {
            buffer_[position_++] = *start++;
        }
    }
}

void LineBasedTarget::flush() {
    // LineBasedTarget doesn't flush() since the concrete classes treat each line independently
    // and upon write(), and often don't require further flushing
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
