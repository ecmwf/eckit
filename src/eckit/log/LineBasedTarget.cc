// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/log/LineBasedTarget.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/maths/Functions.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

LineBasedTarget::LineBasedTarget() : size_(4096), buffer_(new char[size_]), position_(0) {
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
    if (start >= end)
        return;

    reserve(position_ + (end - start) + 1);

    while (start < end) {
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
