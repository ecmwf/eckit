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
    out << "S3Handle[name=" << name_ << ",position=" << pos_ << ", open=" << open_ << ", writable=" << canWrite_ << "]";
}

//----------------------------------------------------------------------------------------------------------------------

void S3Handle::open(const Offset& position) {
    ASSERT(!open_);
    pos_  = position;
    open_ = true;
}

Length S3Handle::openForRead() {
    ASSERT(name_.exists());

    open(0);

    canWrite_ = false;

    return size();
}

void S3Handle::openForWrite(const Length& length) {
    open(Offset(length));

    pos_ += length;

    canWrite_ = true;
}

void S3Handle::openForAppend(const Length& length) {
    open(Offset(length));

    pos_ += length;

    canWrite_ = true;
}

//----------------------------------------------------------------------------------------------------------------------

long S3Handle::read(void* buffer, const long length) {
    if (!open_) { throw S3SeriousBug("S3 handle is not open!", Here()); }

    long len = 0;
    if (pos_ == Offset(0)) {
        len = name_.get(buffer, length);
    } else if (pos_ > Offset(0)) {
        len = name_.get(buffer, pos_, length);
    }

    pos_ += len;

    return len;
}

long S3Handle::write(const void* buffer, const long length) {
    ASSERT(!name_.exists());

    if (!open_ || !canWrite_) { throw S3SeriousBug("S3 handle is not writable!", Here()); }

    const auto len = name_.put(buffer, length);

    pos_ += len;

    return len;
}

void S3Handle::flush() {
    NOTIMP;
}

void S3Handle::close() {
    open_     = false;
    canWrite_ = false;
}

Length S3Handle::size() {
    return name_.size();  // asserts
}

Length S3Handle::estimate() {
    return size();
}

Offset S3Handle::position() {
    return pos_;
}

Offset S3Handle::seek(const Offset& offset) {
    pos_ = pos_ + offset;
    return pos_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
