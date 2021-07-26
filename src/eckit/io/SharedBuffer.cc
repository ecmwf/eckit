/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/SharedBuffer.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

SharedBuffer::SharedBuffer(size_t size) :
    buffer_(new CountedBuffer(size)) {
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
