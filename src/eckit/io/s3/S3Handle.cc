/*
 * Copyright 2024- European Centre for Medium-Range Weather Forecasts (ECMWF).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "eckit/io/s3/S3Handle.h"

#include "eckit/config/LibEcKit.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/s3/S3Exception.h"
#include "eckit/io/s3/S3Name.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

S3Handle::S3Handle(const PathName& name): S3Handle(name, 0) {
    /// @todo remove
    LOG_DEBUG_LIB(LibEcKit) << *this << std::endl;
}

S3Handle::S3Handle(const PathName& name, const Offset& offset): name_(name), pos_(offset) {
    /// @todo remove
    LOG_DEBUG_LIB(LibEcKit) << *this << std::endl;
}

S3Handle::~S3Handle() {
    /// @todo remove
    LOG_DEBUG_LIB(LibEcKit) << *this << std::endl;
}

void S3Handle::print(std::ostream& out) const {
    // out << "S3Handle[" << impl_ << "]";
}

//----------------------------------------------------------------------------------------------------------------------

Length S3Handle::openForRead() {
    /// @todo remove
    LOG_DEBUG_LIB(LibEcKit) << *this << std::endl;

    //     canWrite_ = false;
    //     object_ = name_.object();
    //     if (object_->open()) { return object_->size(); }

    return {};
}

void S3Handle::openForWrite(const Length& length) {
    /// @todo remove
    LOG_DEBUG_LIB(LibEcKit) << *this << std::endl;
    //     canWrite_ = true;
    //     object_ = name_.object();
    //     if (!object_->create(name_.pool())) { throw NotFound("Cannot create object!", -1, Here()); }
}

void S3Handle::openForAppend(const Length& length) {
    /// @todo remove
    LOG_DEBUG_LIB(LibEcKit) << *this << std::endl;
    // NOTIMP;
}

//----------------------------------------------------------------------------------------------------------------------

long S3Handle::read(void* buffer, const long length) {
    /// @todo remove
    LOG_DEBUG_LIB(LibEcKit) << *this << std::endl;
    return 0;
    // return object_ ? object_->read(buffer, length, pos_) : 0;
}

long S3Handle::write(const void* buffer, const long length) {
    /// @todo remove
    LOG_DEBUG_LIB(LibEcKit) << *this << std::endl;
    // if (object_ && canWrite_) { return object_->write(buffer, length, pos_); }
    return 0;
}

void S3Handle::flush() {
    /// @todo remove
    LOG_DEBUG_LIB(LibEcKit) << *this << std::endl;
}

void S3Handle::close() {
    // canWrite_ = false;
    // object_.reset();
    /// @todo remove
    LOG_DEBUG_LIB(LibEcKit) << *this << std::endl;
}

Length S3Handle::size() {
    /// @todo remove
    LOG_DEBUG_LIB(LibEcKit) << *this << std::endl;
    return {};
}

Offset S3Handle::position() {
    /// @todo remove
    LOG_DEBUG_LIB(LibEcKit) << *this << std::endl;
    // return pos_;
    return {};
}

Offset S3Handle::seek(const Offset& offset) {
    // pos_ = pos_ + offset;
    // return pos_;
    /// @todo remove
    LOG_DEBUG_LIB(LibEcKit) << *this << std::endl;
    return {};
}

std::string S3Handle::title() const {
    /// @todo remove
    LOG_DEBUG_LIB(LibEcKit) << *this << std::endl;
    // NOTIMP;
    return {};
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
