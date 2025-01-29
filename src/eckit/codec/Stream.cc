/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/codec/Stream.h"

#include "eckit/codec/Exceptions.h"
#include "eckit/io/DataHandle.h"

namespace eckit::codec {

//---------------------------------------------------------------------------------------------------------------------

Stream::Stream(DataHandle& datahandle) : ptr_(&datahandle) {}

Stream::Stream(DataHandle* datahandle) : shared_(datahandle), ptr_(shared_.get()) {}

Stream::Stream(std::shared_ptr<DataHandle> datahandle) : shared_(datahandle), ptr_(shared_.get()) {}

DataHandle& Stream::datahandle() {
    ASSERT(ptr_ != nullptr);
    return *ptr_;
}

uint64_t Stream::seek(uint64_t offset) {
    ASSERT(ptr_ != nullptr);
    return static_cast<std::uint64_t>(ptr_->seek(static_cast<long long>(offset)));
}

uint64_t Stream::position() {
    ASSERT(ptr_ != nullptr);
    return static_cast<std::uint64_t>(ptr_->position());
}

uint64_t Stream::write(const void* data, size_t length) {
    ASSERT(ptr_ != nullptr);
    return static_cast<std::uint64_t>(ptr_->write(data, static_cast<long>(length)));
}

uint64_t Stream::read(void* data, size_t length) {
    ASSERT(ptr_ != nullptr);
    return static_cast<std::uint64_t>(ptr_->read(data, static_cast<long>(length)));
}

//---------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::codec
