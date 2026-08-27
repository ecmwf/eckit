/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/rados/RadosHandle.h"

#include <rados/librados.h>

#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosObject.h"
#include "eckit/log/Log.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

void RadosHandle::print(std::ostream& s) const {
    s << "RadosHandle[" << object_.str() << ']';
}

RadosHandle::RadosHandle(const RadosObject& object) :
    object_(object), offset_(0), opened_(false), write_(false), first_write_(false) {}

RadosHandle::~RadosHandle() {
    if (opened_) {
        eckit::Log::error() << "RadosHandle not closed before destruction." << std::endl;
    }
}

void RadosHandle::open() {

    ASSERT(!opened_);

    offset_ = 0;

    opened_ = true;
}

Length RadosHandle::size() {
    return RadosCluster::instance().size(object_);
}

Length RadosHandle::openForRead() {

    open();

    write_ = false;

    return RadosCluster::instance().size(object_);
}

void RadosHandle::openForWrite(const Length& length) {

    open();

    write_       = true;
    first_write_ = true;
}

long RadosHandle::read(void* buffer, long length) {

    ASSERT(opened_);
    ASSERT(!write_);

    int len = RADOS_CALL(rados_read(RadosCluster::instance().ioCtx(object_), object_.name().c_str(),
                                    reinterpret_cast<char*>(buffer), length, offset_));

    offset_ += len;

    return len;
}

long RadosHandle::write(const void* buffer, long length) {

    ASSERT(length);
    ASSERT(opened_);
    ASSERT(write_);

    ASSERT(length <= RadosCluster::instance().maxWriteSize());
    ASSERT((offset_ + length) <= RadosCluster::instance().maxObjectSize());

    if (first_write_) {

        RADOS_CALL(rados_write_full(RadosCluster::instance().ioCtx(object_), object_.name().c_str(),
                                    reinterpret_cast<const char*>(buffer), length));

        first_write_ = false;
    }
    else {

        RADOS_CALL(rados_write(RadosCluster::instance().ioCtx(object_), object_.name().c_str(),
                               reinterpret_cast<const char*>(buffer), length, offset_));
    }

    offset_ += length;

    return length;
}

void RadosHandle::flush() {}

eckit::Offset RadosHandle::seek(const eckit::Offset& offset) {
    offset_ = offset;
    return offset_;
}

void RadosHandle::close() {
    ASSERT(opened_);
    opened_ = false;
}

Offset RadosHandle::position() {
    return offset_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
