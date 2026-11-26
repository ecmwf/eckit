/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/io/rados/RadosMultiObjWriteHandle.h"

#include <algorithm>
#include <cstddef>
#include <exception>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <utility>

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/DataHandle.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/io/rados/RadosAsyncHandle.h"
#include "eckit/io/rados/RadosAttributes.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosHandle.h"
#include "eckit/log/Log.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

void RadosMultiObjWriteHandle::print(std::ostream& s) const {
    s << "RadosMultiObjWriteHandle[" << object_.str() << ']';
}

RadosMultiObjWriteHandle::RadosMultiObjWriteHandle(const eckit::RadosObject& obj, bool async, const Length& maxPartSize,
                                                   size_t maxAioBuffSize, size_t maxHandleBuffSize) :
    object_(obj),
    async_(async),
    maxPartSize_(maxPartSize),
    maxAioBuffSize_(maxAioBuffSize),
    maxHandleBuffSize_(maxHandleBuffSize) {

    if (maxPartSize_ == Length(0)) {
        maxPartSize_ = RadosCluster::instance().maxObjectSize();
    }
}

RadosMultiObjWriteHandle::~RadosMultiObjWriteHandle() {
    if (opened_) {
        eckit::Log::error() << "RadosMultiObjWriteHandle not closed before destruction." << std::endl;
        try {
            closeParts(false);
        }
        catch (...) {
            /// @note: nothing can be reported from a destructor
        }
    }
}

Length RadosMultiObjWriteHandle::openForRead() {
    NOTIMP;
}

void RadosMultiObjWriteHandle::openForWrite(const Length& length) {

    ASSERT(!opened_);

    /// @note: parts of a previous, longer version of the object would otherwise be left behind
    object_.ensureAllDestroyed();

    written_  = 0;
    position_ = 0;
    part_     = 0;
    partOpen_ = false;
    opened_   = true;
}

void RadosMultiObjWriteHandle::openForAppend(const Length&) {
    NOTIMP;
}

long RadosMultiObjWriteHandle::read(void* buffer, long length) {

    NOTIMP;
}

void RadosMultiObjWriteHandle::openPart() {

    ASSERT(!partOpen_);

    RadosObject object(object_, part_);
    LOG_DEBUG_LIB(LibEcKit) << "RadosMultiObjWriteHandle::write open " << object.str() << std::endl;

    std::unique_ptr<DataHandle> handle;
    if (async_) {
        handle = std::make_unique<RadosAsyncHandle>(object, maxAioBuffSize_);
    }
    else {
        handle = std::make_unique<RadosHandle>(object);
    }
    handle->openForWrite(0);  // TODO: use proper size

    handles_.push_back(std::move(handle));
    ++part_;
    written_  = 0;
    partOpen_ = true;
}

void RadosMultiObjWriteHandle::closeParts(bool keepCurrent) {

    std::unique_ptr<DataHandle> current;
    if (keepCurrent && partOpen_) {
        current = std::move(handles_.back());
        handles_.pop_back();
    }
    else {
        partOpen_ = false;
    }

    /// @note: close every handle, also if one of them fails, and report the first failure
    std::exception_ptr first;
    for (const auto& handle : handles_) {
        try {
            handle->close();
        }
        catch (...) {
            if (!first) {
                first = std::current_exception();
            }
        }
    }
    handles_.clear();

    if (current) {
        handles_.push_back(std::move(current));
    }

    if (first) {
        std::rethrow_exception(first);
    }
}

long RadosMultiObjWriteHandle::write(const void* buffer, long length) {

    LOG_DEBUG_LIB(LibEcKit) << "RadosMultiObjWriteHandle::write " << length << std::endl;
    ASSERT(opened_);

    long result     = 0;
    const char* buf = reinterpret_cast<const char*>(buffer);

    while (length > 0) {

        if (partOpen_ && Length(written_) >= maxPartSize_) {
            /// @note: asynchronous handles of completed parts stay open, so that their writes
            ///   proceed in the background, until too many of them are held.
            if (!async_ || handles_.size() >= maxHandleBuffSize_) {
                closeParts(false);
            }
            partOpen_ = false;
        }

        if (!partOpen_) {
            openPart();
        }

        /// @note: a part can be larger than what a single write operation accepts
        Length len = std::min(Length(maxPartSize_ - Length(written_)), Length(length));
        len        = std::min(len, RadosCluster::instance().maxWriteSize());
        long l     = (long)len;
        ASSERT(len == Length(l));
        ASSERT(l > 0);

        LOG_DEBUG_LIB(LibEcKit) << "RadosMultiObjWriteHandle::write " << len << " - " << maxPartSize_ << " - "
                                << written_ << std::endl;

        handles_.back()->write(buf + result, l);
        written_ += l;
        result += l;
        length -= l;
        position_ += l;
    }

    return result;
}

void RadosMultiObjWriteHandle::flush() {

    for (const auto& handle : handles_) {
        handle->flush();
    }

    /// @note: completed parts have nothing left to write
    closeParts(true);

    if (part_ > 0) {

        RadosAttributes attrs;

        attrs.set("length", position_);
        attrs.set("parts", part_);
        attrs.set("maxsize", maxPartSize_);

        RadosCluster::instance().attributes(object_, attrs);
    }
}


void RadosMultiObjWriteHandle::close() {

    if (!opened_) {
        return;
    }

    opened_ = false;

    try {
        /// @note: an empty object is stored as a single empty part
        if (part_ == 0) {
            openPart();
        }
        flush();
    }
    catch (...) {
        try {
            closeParts(false);
        }
        catch (...) {
            /// @note: the first failure is the one to report
        }
        throw;
    }

    closeParts(false);
}

void RadosMultiObjWriteHandle::rewind() {
    NOTIMP;
}


Offset RadosMultiObjWriteHandle::position() {
    return position_;
}

std::string RadosMultiObjWriteHandle::title() const {
    return PathName::shorten(object_.str());
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
