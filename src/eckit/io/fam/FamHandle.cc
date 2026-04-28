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
 * This software was developed as part of the Horizon Europe programme funded project OpenCUBE
 * (Grant agreement: 101092984) horizon-opencube.eu
 */

#include "eckit/io/fam/FamHandle.h"

#include <algorithm>
#include <ostream>
#include <sstream>

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/io/fam/FamObject.h"
#include "eckit/io/fam/FamObjectName.h"
#include "eckit/log/Log.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

FamHandle::FamHandle(const FamObjectName& name, const Offset& position, const Length& length, const bool overwrite) :
    name_{name}, overwrite_{overwrite}, pos_{position}, len_{length} {}

FamHandle::FamHandle(const FamObjectName& name, const bool overwrite) : FamHandle(name, 0, 0, overwrite) {}

//----------------------------------------------------------------------------------------------------------------------

void FamHandle::open(const Mode mode) {
    ASSERT(!object_ && mode_ == Mode::CLOSED);
    pos_  = 0;
    mode_ = mode;
}

void FamHandle::close() {
    pos_  = 0;
    mode_ = Mode::CLOSED;
    object_.reset();
}

void FamHandle::flush() {
    Log::debug<LibEcKit>() << "FamHandle::flush() ?! noop\n";
}

Offset FamHandle::seek(const Offset& offset) {
    pos_ = offset;

    ASSERT(0 <= pos_ && size() >= pos_);

    return pos_;
}

Length FamHandle::size() {
    return object_ ? Length(object_->size()) : estimate();
}

//----------------------------------------------------------------------------------------------------------------------

Length FamHandle::openForRead() {
    open(Mode::READ);
    object_ = name_.lookup();
    len_    = size();
    return len_;
}

void FamHandle::openForWrite(const Length& length) {
    open(Mode::WRITE);

    try {
        object_ = name_.lookup();
        if (overwrite_ && length > 0) {
            ASSERT(size() >= length);
        }
    }
    catch (const NotFound& e) {
        Log::debug<LibEcKit>() << "FamHandle::openForWrite() " << e.what() << '\n';
        ASSERT(length > 0);
        object_ = name_.allocate(length);
    }

    len_ = size();
}

//----------------------------------------------------------------------------------------------------------------------

long FamHandle::read(void* buffer, long length) {
    ASSERT(mode_ == Mode::READ);
    ASSERT(0 <= pos_);

    if (size() <= pos_) {
        return 0;
    }

    // Adjust length to read only up to the end of the object
    length = std::min<long>(len_ - pos_, length);

    object_->get(buffer, pos_, length);

    pos_ += length;

    return length;
}

long FamHandle::write(const void* buffer, const long length) {
    ASSERT(mode_ == Mode::WRITE);
    ASSERT(0 <= pos_);
    ASSERT(length >= 0);

    const auto end = static_cast<fam::size_t>(pos_) + static_cast<fam::size_t>(length);
    if (end > object_->size()) {
        std::ostringstream msg;
        msg << "FamHandle::write: out of range for object '" << name_ << "': pos=" << pos_ << ", length=" << length
            << ", object size=" << object_->size();
        throw OutOfRange(msg.str(), Here());
    }

    object_->put(buffer, pos_, length);

    pos_ += length;

    return length;
}

//----------------------------------------------------------------------------------------------------------------------

void FamHandle::print(std::ostream& out) const {
    out << "FamHandle[name=" << name_ << ", position=" << pos_ << ", mode=";
    switch (mode_) {
        case Mode::CLOSED:
            out << "closed";
            break;
        case Mode::READ:
            out << "read";
            break;
        case Mode::WRITE:
            out << "write";
            break;
    }
    out << "]";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
