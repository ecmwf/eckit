/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/rados/RadosMultiObjReadHandle.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/MultiHandle.h"
#include "eckit/io/rados/RadosAttributes.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosHandle.h"

namespace eckit {


// ClassSpec RadosMultiObjReadHandle::classSpec_ = {
//     &DataHandle::classSpec(),
//     "RadosMultiObjReadHandle",
// };
// Reanimator<RadosMultiObjReadHandle> RadosMultiObjReadHandle::reanimator_;

void RadosMultiObjReadHandle::print(std::ostream& s) const {
    s << "RadosMultiObjReadHandle[" << object_.str() << ']';
}

// void RadosMultiObjReadHandle::encode(Stream& s) const {
//     DataHandle::encode(s);
//     s << object_;
// }

// RadosMultiObjReadHandle::RadosMultiObjReadHandle(Stream& s) :
//     DataHandle(s), object_(s) {}

RadosMultiObjReadHandle::RadosMultiObjReadHandle(const eckit::RadosObject& obj) : object_(obj) {}

// RadosMultiObjReadHandle::RadosMultiObjReadHandle(const eckit::URI& uri) : 
//     RadosMultiObjReadHandle(eckit::RadosObject(uri)) {}

RadosMultiObjReadHandle::~RadosMultiObjReadHandle() {}

Length RadosMultiObjReadHandle::estimate() {
    if (length_ == Length(0)) {
        RadosAttributes attr = RadosCluster::instance().attributes(object_);
        ASSERT(attr.get("length", length_));
    }
    return length_;
}


Length RadosMultiObjReadHandle::openForRead() {
    ASSERT(!handle_);

    RadosAttributes attr = RadosCluster::instance().attributes(object_);
    std::cout << "Attributes for " << object_.str() << " ===> " << attr << std::endl;

    ASSERT(attr.get("length", length_));
    ASSERT(attr.get("parts", parts_));

    handle_.reset(new MultiHandle());
    for (size_t i = 0; i < parts_; ++i) {
        (*handle_) += new RadosHandle(RadosObject(object_, i));
    }

    Length len = handle_->openForRead();

    std::cout << "RadosMultiObjReadHandle::openForRead attr=" << length_ << ", open=" << len << std::endl;
    ASSERT(len == length_);

    return length_;
}

void RadosMultiObjReadHandle::openForWrite(const Length& length) {
    NOTIMP;
}

void RadosMultiObjReadHandle::openForAppend(const Length&) {
    NOTIMP;
}

long RadosMultiObjReadHandle::read(void* buffer, long length) {
    ASSERT(handle_);
    return handle_->read(buffer, length);
}

long RadosMultiObjReadHandle::write(const void* buffer, long length) {
    NOTIMP;
}

void RadosMultiObjReadHandle::flush() {
    NOTIMP;
}

eckit::Offset RadosMultiObjReadHandle::seek(const eckit::Offset& offset) {
    ASSERT(handle_);
    return handle_->seek(offset);
}

void RadosMultiObjReadHandle::close() {
    if (handle_) {
        handle_->close();
        handle_.reset(0);
    }
}

void RadosMultiObjReadHandle::rewind() {
    NOTIMP;
}


Offset RadosMultiObjReadHandle::position() {
    NOTIMP;
}

std::string RadosMultiObjReadHandle::title() const {
    return PathName::shorten(object_.str());
}

}  // namespace eckit
