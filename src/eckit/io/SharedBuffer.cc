// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/SharedBuffer.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

SharedBuffer::SharedBuffer(size_t size) : buffer_(new CountedBuffer(size)) {
    ASSERT(buffer_);
    buffer_->attach();
}

SharedBuffer::SharedBuffer(CountedBuffer* b) {
    ASSERT(b);
    buffer_ = b;
    buffer_->attach();
}

void SharedBuffer::print(std::ostream& os) const {
    os << "SharedBuffer(address=" << buffer_ << ",count=" << buffer_->count() << ")";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
