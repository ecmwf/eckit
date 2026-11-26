// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/io/rados/RadosMultiObjReadHandle.h"

#include <rados/librados.h>

#include <algorithm>
#include <cstddef>
#include <ostream>
#include <string>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/io/rados/RadosCall.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosException.h"
#include "eckit/io/rados/RadosMultiObject.h"
#include "eckit/io/rados/RadosObject.h"
#include "eckit/log/CodeLocation.h"
#include "eckit/log/Log.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

void RadosMultiObjReadHandle::print(std::ostream& s) const {
    s << "RadosMultiObjReadHandle[" << object_.str() << ']';
}

RadosMultiObjReadHandle::RadosMultiObjReadHandle(const eckit::RadosObject& obj) : object_(obj) {}

RadosMultiObjReadHandle::~RadosMultiObjReadHandle() {
    if (opened_) {
        eckit::Log::error() << "RadosMultiObjReadHandle not closed before destruction." << std::endl;
    }
}

void RadosMultiObjReadHandle::load() {

    if (loaded_) {
        return;
    }

    const auto layout = RadosMultiObject(object_).layout();
    if (!layout) {
        if (!object_.exists()) {
            throw NotFound("RADOS object " + object_.str(), Here(), true);
        }
        throw RadosException("RADOS object " + object_.str() + " was not written as a multipart object", Here());
    }

    if (!(layout->maxPartSize > Length(0))) {
        throw RadosException("RADOS object " + object_.str() + " has an invalid multipart layout", Here());
    }

    length_      = layout->length;
    maxPartSize_ = layout->maxPartSize;

    loaded_ = true;
}

Length RadosMultiObjReadHandle::estimate() {
    load();
    return length_;
}

Length RadosMultiObjReadHandle::size() {
    load();
    return length_;
}

Length RadosMultiObjReadHandle::openForRead() {
    ASSERT(!opened_);

    /// @note: the object may have been replaced since the attributes were last read
    loaded_ = false;
    load();

    position_ = 0;
    opened_   = true;

    return length_;
}

/// @note: every part but the last holds maxPartSize_ bytes, so the part holding a given
///   position is known without asking the cluster for the size of each of them.
long RadosMultiObjReadHandle::read(void* buffer, long length) {

    ASSERT(opened_);

    const long long total   = length_;
    const long long maxPart = maxPartSize_;

    char* buf   = reinterpret_cast<char*>(buffer);
    long result = 0;

    while (length > 0 && static_cast<long long>(position_) < total) {

        const long long pos    = position_;
        const size_t part      = pos / maxPart;
        const long long offset = pos % maxPart;

        /// @note: librados rejects reads larger than INT_MAX
        const long long len = std::min<long long>({length, maxPart - offset, total - pos, radosMaxReadSize});

        const RadosObject object = RadosMultiObject(object_).part(part);

        int read = RADOS_CALL(
            rados_read(RadosCluster::instance().ioCtx(object), object.name().c_str(), buf + result, len, offset));

        if (read == 0) {
            break;
        }

        result += read;
        length -= read;
        position_ += read;
    }

    return result;
}

eckit::Offset RadosMultiObjReadHandle::seek(const eckit::Offset& offset) {
    ASSERT(opened_);
    ASSERT(offset >= Offset(0));
    position_ = offset;
    return position_;
}

void RadosMultiObjReadHandle::close() {
    opened_ = false;
    loaded_ = false;
}

void RadosMultiObjReadHandle::rewind() {
    ASSERT(opened_);
    position_ = 0;
}


Offset RadosMultiObjReadHandle::position() {
    ASSERT(opened_);
    return position_;
}

std::string RadosMultiObjReadHandle::title() const {
    return PathName::shorten(object_.str());
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
