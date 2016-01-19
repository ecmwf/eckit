/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/DataBlob.h"
#include "eckit/memory/MemoryPool.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

DataBlob::DataBlob(size_t length) :
    buffer_(length) {
}

DataBlob::DataBlob(const void* data, size_t length) :
    buffer_((const char*)data, length) {
}

DataBlob::~DataBlob() {
}

const Buffer& DataBlob::buffer() const {
    return buffer_;
}

Buffer& DataBlob::buffer() {
    return buffer_;
}

size_t DataBlob::length() const { return buffer_.size(); }

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

