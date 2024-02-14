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
#include "eckit/io/s3/S3Exception.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

S3Handle::S3Handle(const S3Name& name): S3Handle(name, 0) { }

S3Handle::S3Handle(const S3Name& name, const Offset& offset): name_(name), pos_(offset) { }

void S3Handle::print(std::ostream& out) const {
    out << "S3Handle[name=" << name_ << ", position=" << pos_ << ", open=" << open_ << ", writable=" << canWrite_ << "]";
}

//----------------------------------------------------------------------------------------------------------------------

void S3Handle::open(const bool canWrite) {
    ASSERT(!open_);
    pos_      = 0;
    open_     = true;
    canWrite_ = canWrite;
}

Length S3Handle::openForRead() {
    open(false);

    return size();
}

void S3Handle::openForWrite(const Length& length) {
    open(true);

    if (name_.exists()) { ASSERT(size() == length); }
}

void S3Handle::openForAppend(const Length& length) {
    open(true);

    const auto oSize = size();

    ASSERT(oSize == length);

    pos_ += oSize;
}

//----------------------------------------------------------------------------------------------------------------------

long S3Handle::read(void* buffer, const long length) {
    ASSERT(open_);
    ASSERT(!canWrite_);

    const auto oSize = size();

    if (pos_ == oSize) { return 0; }

    const auto rLength = name_.get(buffer, pos_, length);

    pos_ += rLength;

    ASSERT(pos_ >= Offset(0) || pos_ <= oSize);

    return rLength;
}

long S3Handle::write(const void* buffer, const long length) {
    ASSERT(open_);
    ASSERT(canWrite_);

    const auto rLength = name_.put(buffer, length);

    pos_ += rLength;

    ASSERT(pos_ >= Offset(0) || pos_ <= size());

    return rLength;
}

void S3Handle::flush() {
    NOTIMP;
}

void S3Handle::close() {
    pos_      = 0;
    open_     = false;
    canWrite_ = false;
}

Length S3Handle::size() {
    return name_.size();
}

Length S3Handle::estimate() {
    return size();
}

Offset S3Handle::position() {
    return pos_;
}

Offset S3Handle::seek(const Offset& offset) {
    pos_ = pos_ + offset;
    ASSERT(pos_ >= Offset(0) || pos_ <= size());
    return pos_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
