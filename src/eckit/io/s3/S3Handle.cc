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
    out << "S3Handle[name=" << name_ << ",position=" << pos_ << "]";
}

//----------------------------------------------------------------------------------------------------------------------

Length S3Handle::openForRead() {
    canWrite_ = false;
    return name_.size();  // asserts
}

void S3Handle::openForWrite(const Length&) {
    canWrite_ = true;
}

void S3Handle::openForAppend(const Length&) {
    NOTIMP;
    canWrite_ = true;
}

//----------------------------------------------------------------------------------------------------------------------

long S3Handle::read(void* buffer, const long length) {
    /// @todo remove
    LOG_DEBUG_LIB(LibEcKit) << *this << std::endl;
    return 0;
}

long S3Handle::write(const void* buffer, const long length) {
    name_.put(buffer, length);
    pos_ += length;
    return length;
}

void S3Handle::flush() {
    NOTIMP;
}

void S3Handle::close() {
    /// @todo remove
    LOG_DEBUG_LIB(LibEcKit) << "close!" << std::endl;
    canWrite_ = false;
    // object_.reset();
}

Length S3Handle::size() {
    return name_.size();  // asserts
}

Offset S3Handle::position() {
    return pos_;
}

Offset S3Handle::seek(const Offset& offset) {
    pos_ = pos_ + offset;
    return pos_;
}

std::string S3Handle::title() const {
    return name_.asString();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
