// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
