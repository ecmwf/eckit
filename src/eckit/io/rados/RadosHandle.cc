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

#include "eckit/exception/Exceptions.h"

namespace eckit {


ClassSpec RadosHandle::classSpec_ = {
    &DataHandle::classSpec(),
    "RadosHandle",
};

Reanimator<RadosHandle> RadosHandle::reanimator_;

void RadosHandle::print(std::ostream& s) const {
    s << "RadosHandle[" << object_ << ']';
}

void RadosHandle::encode(Stream& s) const {
    DataHandle::encode(s);
    s << object_;
}

RadosHandle::RadosHandle(Stream& s) :
    DataHandle(s), object_(s), offset_(0), opened_(false), write_(false) {}

RadosHandle::RadosHandle(const RadosObject& object) :
    object_(object), offset_(0), opened_(false), write_(false) {}


RadosHandle::RadosHandle(const std::string& object) :
    object_(object), offset_(0), opened_(false), write_(false) {}

RadosHandle::~RadosHandle() {
    // std::cout << "RadosHandle::~RadosHandle " << object_ << std::endl;

    if (opened_) {
        close();
    }
}

void RadosHandle::open() {

    // std::cout << "RadosHandle::open " << object_ << std::endl;

    ASSERT(!opened_);

    offset_ = 0;

    opened_ = true;
}

Length RadosHandle::estimate() {
    return RadosCluster::instance().size(object_);
}

Length RadosHandle::openForRead() {

    // std::cout << "RadosHandle::openForRead " << object_ << std::endl;

    open();
    write_ = false;

    return RadosCluster::instance().size(object_);
}

void RadosHandle::openForWrite(const Length& length) {

    // std::cout << "RadosHandle::openForWrite " << object_ << " " << length << std::endl;

    RadosCluster::instance().ensurePool(object_);
    RadosCluster::instance().truncate(object_);

    open();
    write_ = true;
}

void RadosHandle::openForAppend(const Length&) {
    NOTIMP;
}

long RadosHandle::read(void* buffer, long length) {

    // std::cout << "RadosHandle::read " << object_ << " " << length << std::endl;

    ASSERT(opened_);
    ASSERT(!write_);

    long maxLength = RadosCluster::instance().maxObjectSize();

    long readLength = length > maxLength ? maxLength : length;

    int len = RADOS_CALL(rados_read(RadosCluster::instance().ioCtx(object_), object_.oid().c_str(),
                                    reinterpret_cast<char*>(buffer), readLength, offset_));
    // ASSERT(len  > 0);

    offset_ += len;

    return len;
}

long RadosHandle::write(const void* buffer, long length) {

    ASSERT(length);

    // std::cout << "RadosHandle::write " << object_ << " " << length << std::endl;

    ASSERT(opened_);
    ASSERT(write_);

    RADOS_CALL(rados_write(RadosCluster::instance().ioCtx(object_), object_.oid().c_str(),
                           reinterpret_cast<const char*>(buffer), length, offset_));

    offset_ += length;

    return length;
}

void RadosHandle::flush() {
    // NOTIMP;
}

void RadosHandle::close() {
    std::cout << "RadosHandle::close " << object_ << std::endl;
    ASSERT(opened_);
    opened_ = false;
}

void RadosHandle::rewind() {
    offset_ = 0;
}


Offset RadosHandle::position() {
    return offset_;
}

std::string RadosHandle::title() const {
    return PathName::shorten(object_.str());
}

}  // namespace eckit
