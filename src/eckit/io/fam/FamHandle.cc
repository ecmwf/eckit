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
#include "eckit/io/fam/FamName.h"
#include "eckit/log/Log.h"

#include <utility>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

FamHandle::FamHandle(const FamName& name, const Offset& position, const Length& length, const bool overwrite):
    name_ {name}, pos_ {position}, len_ {length}, overwrite_ {overwrite} { }

FamHandle::FamHandle(const FamName& name, const bool overwrite): FamHandle(name, 0, 0, overwrite) { }

//----------------------------------------------------------------------------------------------------------------------

void FamHandle::open(const Mode mode) {
    ASSERT(mode_ == Mode::CLOSED);
    pos_  = 0;
    mode_ = mode;
}

void FamHandle::close() {
    if (mode_ == Mode::WRITE) { flush(); }
    mode_ = Mode::CLOSED;
    pos_  = 0;
    handle_.reset();
}

void FamHandle::flush() {
    Log::debug<LibEcKit>() << "FamHandle::flush() ?! noop\n";
}

Offset FamHandle::seek(const Offset& offset) {
    pos_ = pos_ + offset;

    ASSERT(0 <= pos_ && size() >= pos_);

    return pos_;
}

Length FamHandle::size() {
    return handle_ ? Length(handle_->size()) : estimate();
}

//----------------------------------------------------------------------------------------------------------------------

Length FamHandle::openForRead() {
    open(Mode::READ);
    handle_ = name_.lookupObject().clone();
    return estimate();
}

void FamHandle::openForWrite(const Length& length) {
    open(Mode::WRITE);

    try {
        handle_ = name_.lookupObject().clone();
        if (overwrite_ && length > 0) { ASSERT(size() >= length); }
    } catch (const NotFound& e) {
        Log::debug<LibEcKit>() << "FamHandle::openForWrite() " << e.what() << '\n';
        ASSERT(length > 0);
        handle_ = name_.allocateObject(length).clone();
    }

    len_ = size();

    // try {
    //     name_.create(static_cast<fam::size_t>(length));
    // } catch (const AlreadyExists& e) {
    //     Log::debug<LibEcKit>() << "FamHandle::openForWrite() " << e.what() << '\n';
    //     ASSERT(overwrite_ && length == size());
    //     name_.object()->deallocate();
    //     name_.create(static_cast<fam::size_t>(length));
    // }

    /// @todo slow code, use of length ?
    // if (length > 0 && name_.exists()) { ASSERT(size() == length); }
}

//----------------------------------------------------------------------------------------------------------------------

long FamHandle::read(void* buffer, const long length) {
    ASSERT(mode_ == Mode::READ);
    ASSERT(0 <= pos_);

    if (size() <= pos_) { return 0; }

    handle_->get(buffer, pos_, length);
    // const auto len = name_.get(buffer, pos_, length);

    pos_ += length;

    return length;
}

long FamHandle::write(const void* buffer, const long length) {
    ASSERT(mode_ == Mode::WRITE);

    handle_->put(buffer, pos_, length);
    // const auto len = name_.put(buffer, length);

    pos_ += length;

    return length;
}

//----------------------------------------------------------------------------------------------------------------------

void FamHandle::print(std::ostream& out) const {
    out << "FamHandle[name=" << name_ << ", position=" << pos_ << ", mode=";
    switch (mode_) {
        case Mode::CLOSED: out << "closed"; break;
        case Mode::READ:   out << "read"; break;
        case Mode::WRITE:  out << "write"; break;
    }
    out << "]";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
