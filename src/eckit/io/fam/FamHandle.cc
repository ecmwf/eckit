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

#include "eckit/io/fam/FamHandle.h"

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"

#include <utility>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

FamHandle::FamHandle(FamObjectName name, const Offset& offset, const Length& length):
    name_ {std::move(name)}, pos_ {offset}, len_ {length} { }

FamHandle::FamHandle(FamObjectName name, const Offset& offset): FamHandle(std::move(name), offset, 0) { }

void FamHandle::print(std::ostream& out) const {
    out << "FamHandle[name=" << name_ << ",position=" << pos_ << ",mode=";
    switch (mode_) {
        case Mode::CLOSED: out << "closed"; break;
        case Mode::READ:   out << "read"; break;
        case Mode::WRITE:  out << "write"; break;
    }
    out << "]";
}

//----------------------------------------------------------------------------------------------------------------------

Length FamHandle::openForRead() {
    open(Mode::READ);

    return estimate();
}

void FamHandle::openForWrite(const Length& /*length*/) {
    open(Mode::WRITE);

    /// @todo any checks ?
    // ASSERT(name_.bucketExists());

    /// @todo slow code, use of length ?
    // if (length > 0 && name_.exists()) { ASSERT(size() == length); }
}

//----------------------------------------------------------------------------------------------------------------------

long FamHandle::read(void* buffer, const long length) {
    ASSERT(mode_ == Mode::READ);
    ASSERT(0 <= pos_);

    if (size() <= pos_) { return 0; }

    name_.get(buffer, pos_, length);
    // const auto len = name_.get(buffer, pos_, length);

    pos_ += length;

    return length;
}

long FamHandle::write(const void* buffer, const long length) {
    ASSERT(mode_ == Mode::WRITE);
    ASSERT(!name_.exists());

    name_.put(buffer, pos_, length);
    // const auto len = name_.put(buffer, length);

    pos_ += length;

    return length;
}

//----------------------------------------------------------------------------------------------------------------------

void FamHandle::open(const Mode mode) {
    ASSERT(mode_ == Mode::CLOSED);
    pos_  = 0;
    mode_ = mode;
}

void FamHandle::close() {
    if (mode_ == Mode::WRITE) { flush(); }
    pos_  = 0;
    mode_ = Mode::CLOSED;
}

void FamHandle::flush() {
    Log::debug<LibEcKit>() << "flushed?! noop\n";
}

//----------------------------------------------------------------------------------------------------------------------

Length FamHandle::size() {
    return name_.size();
}

Length FamHandle::estimate() {
    return size();
}

Offset FamHandle::seek(const Offset& offset) {
    pos_ = pos_ + offset;

    ASSERT(0 <= pos_ && size() >= pos_);

    return pos_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
