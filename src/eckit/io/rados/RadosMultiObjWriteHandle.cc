// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/io/rados/RadosMultiObjWriteHandle.h"

#include <algorithm>
#include <cstddef>
#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/DataHandle.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/io/rados/RadosCall.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosMultiObject.h"
#include "eckit/io/rados/RadosObject.h"
#include "eckit/log/Log.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

void RadosMultiObjWriteHandle::print(std::ostream& s) const {
    s << "RadosMultiObjWriteHandle[" << object_.str() << ']';
}

RadosMultiObjWriteHandle::RadosMultiObjWriteHandle(const eckit::RadosObject& obj, const Length& maxPartSize,
                                                   RadosWriteMode mode, size_t maxPendingBytes, size_t maxOpenParts) :
    object_(obj),
    maxPartSize_(maxPartSize == Length(0) ? RadosCluster::instance().maxObjectSize() : maxPartSize),
    mode_(mode),
    maxPendingBytes_(maxPendingBytes),
    maxOpenParts_(maxOpenParts) {}

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

void RadosMultiObjWriteHandle::openPart() {

    ASSERT(!partOpen_);

    const RadosObject object = RadosMultiObject(object_).part(part_);
    LOG_DEBUG_LIB(LibEcKit) << "RadosMultiObjWriteHandle::write open " << object.str() << std::endl;

    std::unique_ptr<DataHandle> handle(object.dataHandle(mode_, maxPendingBytes_));
    handle->openForWrite(0);

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

    /// @note: the handles are given up whether closing them succeeds or not
    std::vector<std::unique_ptr<DataHandle>> closing;
    closing.swap(handles_);

    if (current) {
        handles_.push_back(std::move(current));
    }

    forAllThenRethrowFirst(closing, [](const std::unique_ptr<DataHandle>& handle) { handle->close(); });
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
            if (mode_ == RadosWriteMode::Sync || handles_.size() >= maxOpenParts_) {
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
        RadosMultiObject::Layout layout;
        layout.length      = Length(static_cast<long long>(position_));
        layout.parts       = part_;
        layout.maxPartSize = maxPartSize_;
        RadosMultiObject(object_).layout(layout);
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


Offset RadosMultiObjWriteHandle::position() {
    return position_;
}

std::string RadosMultiObjWriteHandle::title() const {
    return PathName::shorten(object_.str());
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
