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

void S3Handle::openForWrite(const Length& length) {
    open(Mode::WRITE);

    ASSERT(name_.bucketExists());

    if (name_.exists()) { ASSERT(size() == length); }
}

//----------------------------------------------------------------------------------------------------------------------

long S3Handle::read(void* buffer, const long length) {
    ASSERT(mode_ == Mode::READ);

    if (size() == pos_) { return 0; }

    return seek(name_.get(buffer, pos_, length));
}

long S3Handle::write(const void* buffer, const long length) {
    ASSERT(mode_ == Mode::WRITE);
    ASSERT(!name_.exists());

    return seek(name_.put(buffer, length));
}

//----------------------------------------------------------------------------------------------------------------------

void S3Handle::open(const Mode mode) {
    ASSERT(mode_ == Mode::CLOSED);
    pos_  = 0;
    mode_ = mode;
}

void S3Handle::close() {
    pos_  = 0;
    mode_ = Mode::CLOSED;
}

void S3Handle::flush() {
    NOTIMP;
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
