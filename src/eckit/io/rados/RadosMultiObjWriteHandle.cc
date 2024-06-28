/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <map>

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/rados/RadosAttributes.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosHandle.h"
#include "eckit/io/rados/RadosAsyncHandle.h"
#include "eckit/io/rados/RadosMultiObjWriteHandle.h"


namespace eckit {


// ClassSpec RadosMultiObjWriteHandle::classSpec_ = {
//     &DataHandle::classSpec(),
//     "RadosMultiObjWriteHandle",
// };
// Reanimator<RadosMultiObjWriteHandle> RadosMultiObjWriteHandle::reanimator_;

void RadosMultiObjWriteHandle::print(std::ostream& s) const {
    s << "RadosMultiObjWriteHandle[" << object_.str() << ']';
}

// void RadosMultiObjWriteHandle::encode(Stream& s) const {
//     DataHandle::encode(s);
//     s << object_;
//     s << Length(0);  // For future extensio
// }

// RadosMultiObjWriteHandle::RadosMultiObjWriteHandle(Stream& s) :
//     DataHandle(s), object_(s), opened_(false) {
//     s >> maxObjectSize_;
//     if (!maxObjectSize_) {
//         maxObjectSize_ = RadosCluster::instance().maxObjectSize();
//     }
// }

RadosMultiObjWriteHandle::RadosMultiObjWriteHandle(const eckit::RadosObject& obj, bool async, 
    const Length& maxObjectSize, size_t maxAioBuffSize, size_t maxHandleBuffSize) : 
    object_(obj), async_(async), maxObjectSize_(maxObjectSize), 
    maxAioBuffSize_(maxAioBuffSize), maxHandleBuffSize_(maxHandleBuffSize), opened_(false) {

    if (!maxObjectSize_) {
        maxObjectSize_ = RadosCluster::instance().maxObjectSize();
    }

}

// RadosMultiObjWriteHandle::RadosMultiObjWriteHandle(const eckit::URI& uri, const Length& maxObjectSize) :
//     RadosMultiObjWriteHandle(eckit::RadosObject(uri), maxObjectSize) {}

RadosMultiObjWriteHandle::~RadosMultiObjWriteHandle() {}

Length RadosMultiObjWriteHandle::openForRead() {
    NOTIMP;
}

void RadosMultiObjWriteHandle::openForWrite(const Length& length) {

    ASSERT(!opened_);

    written_  = 0;
    position_ = 0;
    part_     = 0;
    opened_   = true;

    /// @note: if the RadosMultiObjWriteHandle is configured to be async (via bool async),
    ///   a vector of open handles will be kept until flush or close.
    ///   If it is configured to not be async, a single handle will be kept in the handles_ vector.
    ///   Either way, the algorithm requires that the vector is initialised with an empty unique_ptr.
    handles_.push_back(std::unique_ptr<DataHandle>(nullptr));

}

void RadosMultiObjWriteHandle::openForAppend(const Length&) {
    NOTIMP;
}

long RadosMultiObjWriteHandle::read(void* buffer, long length) {

    NOTIMP;
}

long RadosMultiObjWriteHandle::write(const void* buffer, long length) {

    LOG_DEBUG_LIB(LibEcKit) << "RadosMultiObjWriteHandle::write " << length << std::endl;
    ASSERT(opened_);

    if (length == 0) {
        return 0;
    }


    long result     = 0;
    const char* buf = reinterpret_cast<const char*>(buffer);

    while (length > 0) {

        Length len = std::min(Length(maxObjectSize_ - Length(written_)), Length(length));
        long l     = (long)len;
        ASSERT(len == Length(l));

        if (l == 0) {
            ASSERT(handles_.back());
            if (!async_) {
                handles_.back()->close();
                handles_.back().reset(0);
            } else {
                ASSERT(handles_.size() < maxHandleBuffSize_);
                handles_.push_back(std::unique_ptr<DataHandle>(nullptr));
            }
            written_ = 0;
            continue;
        }

        LOG_DEBUG_LIB(LibEcKit) << "RadosMultiObjWriteHandle::write " << len << " - " << maxObjectSize_ << " - " << written_ << std::endl;

        if (!handles_.back().get()) {

            RadosObject object(object_, part_++);
            LOG_DEBUG_LIB(LibEcKit) << "RadosMultiObjWriteHandle::write open " << object.str() << std::endl;
            if (async_) {
                handles_.back().reset(new RadosAsyncHandle(object, maxAioBuffSize_));
            } else {
                handles_.back().reset(new RadosHandle(object));
            }
            handles_.back()->openForWrite(0);  // TODO: use proper size
        }


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

}


void RadosMultiObjWriteHandle::close() {

    for (const auto& handle : handles_) {
        handle->close();
    }
    handles_.clear();

    if (opened_) {

        RadosAttributes attrs;

        attrs.set("length", position_);
        attrs.set("parts", part_);
        attrs.set("maxsize", maxObjectSize_);

        RadosCluster::instance().attributes(object_, attrs);

        opened_ = false;
    }
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

}  // namespace eckit
