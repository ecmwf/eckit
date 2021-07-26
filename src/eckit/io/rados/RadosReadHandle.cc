/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/rados/RadosReadHandle.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/MultiHandle.h"
#include "eckit/io/rados/RadosAttributes.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosHandle.h"

namespace eckit {


ClassSpec RadosReadHandle::classSpec_ = {
    &DataHandle::classSpec(),
    "RadosReadHandle",
};
Reanimator<RadosReadHandle> RadosReadHandle::reanimator_;

void RadosReadHandle::print(std::ostream& s) const {
    s << "RadosReadHandle[" << object_ << ']';
}

void RadosReadHandle::encode(Stream& s) const {
    DataHandle::encode(s);
    s << object_;
}

RadosReadHandle::RadosReadHandle(Stream& s) :
    DataHandle(s), object_(s) {}

RadosReadHandle::RadosReadHandle(const std::string& name) :
    object_(name) {}

RadosReadHandle::~RadosReadHandle() {}

Length RadosReadHandle::estimate() {
    if (length_ == Length(0)) {
        RadosAttributes attr = RadosCluster::instance().attributes(object_);
        ASSERT(attr.get("length", length_));
    }
    return length_;
}


Length RadosReadHandle::openForRead() {
    ASSERT(!handle_);

    RadosAttributes attr = RadosCluster::instance().attributes(object_);
    std::cout << "Attributes for " << object_ << " ===> " << attr << std::endl;

    ASSERT(attr.get("length", length_));
    ASSERT(attr.get("parts", parts_));

    handle_.reset(new MultiHandle());
    for (size_t i = 0; i < parts_; ++i) {
        (*handle_) += new RadosHandle(RadosObject(object_, i));
    }

    Length len = handle_->openForRead();

    std::cout << "RadosReadHandle::openForRead attr=" << length_ << ", open=" << len << std::endl;
    ASSERT(len == length_);

    return length_;
}

void RadosReadHandle::openForWrite(const Length& length) {
    NOTIMP;
}

void RadosReadHandle::openForAppend(const Length&) {
    NOTIMP;
}

long RadosReadHandle::read(void* buffer, long length) {
    ASSERT(handle_);
    return handle_->read(buffer, length);
}

long RadosReadHandle::write(const void* buffer, long length) {
    NOTIMP;
}

void RadosReadHandle::flush() {
    NOTIMP;
}


void RadosReadHandle::close() {
    if (handle_) {
        handle_->close();
        handle_.reset(0);
    }
}

void RadosReadHandle::rewind() {
    NOTIMP;
}


Offset RadosReadHandle::position() {
    NOTIMP;
}

std::string RadosReadHandle::title() const {
    return PathName::shorten(object_.str());
}

}  // namespace eckit
