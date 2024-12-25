/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/*
 * This software was developed as part of the EC H2020 funded project IO-SEA
 * (Project ID: 955811) iosea-project.eu
 */

#include "eckit/io/s3/S3Handle.h"

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/io/s3/S3ObjectName.h"
#include "eckit/log/Log.h"

#include <ostream>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

S3Handle::S3Handle(const S3ObjectName& name, const Offset& offset) : name_(name), pos_(offset) { }

S3Handle::S3Handle(const S3ObjectName& name) : S3Handle(name, 0) { }

void S3Handle::print(std::ostream& out) const {
    out << "S3Handle[name=" << name_ << ", position=" << pos_;
    if (mode_ == Mode::CLOSED) {
        out << ", mode=closed";
    } else if (mode_ == Mode::READ) {
        out << ", mode=read";
    } else if (mode_ == Mode::WRITE) {
        out << ", mode=write";
    }
    out << "]";
}

//----------------------------------------------------------------------------------------------------------------------

Length S3Handle::openForRead() {
    open(Mode::READ);

    return estimate();
}

void S3Handle::openForWrite(const Length& /*length*/) {
    open(Mode::WRITE);

    ASSERT(name_.bucketExists());

    /// @todo slow code, use of length ?
    // if (length > 0 && name_.exists()) { ASSERT(size() == length); }
}

//----------------------------------------------------------------------------------------------------------------------

long S3Handle::read(void* buffer, const long length) {
    ASSERT(mode_ == Mode::READ);
    ASSERT(0 <= pos_);

    if (size() <= pos_) { return 0; }

    const auto len = name_.get(buffer, pos_, length);

    pos_ += len;

    return len;
}

long S3Handle::write(const void* buffer, const long length) {
    ASSERT(mode_ == Mode::WRITE);
    ASSERT(!name_.exists());

    const auto len = name_.put(buffer, length);

    pos_ += len;

    return len;
}

//----------------------------------------------------------------------------------------------------------------------

void S3Handle::open(const Mode mode) {
    ASSERT(mode_ == Mode::CLOSED);
    pos_  = 0;
    mode_ = mode;
}

void S3Handle::close() {
    if (mode_ == Mode::WRITE) { flush(); }
    pos_  = 0;
    mode_ = Mode::CLOSED;
}

void S3Handle::flush() {
    LOG_DEBUG_LIB(LibEcKit) << "flush()!" << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

Length S3Handle::size() {
    return name_.size();
}

Length S3Handle::estimate() {
    return size();
}

Offset S3Handle::seek(const Offset& offset) {
    pos_ = pos_ + offset;

    ASSERT(0 <= pos_ && size() >= pos_);

    return pos_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit