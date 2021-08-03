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

#include "eckit/exception/Exceptions.h"
#include "eckit/io/rados/RadosAttributes.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosHandle.h"
#include "eckit/io/rados/RadosWriteHandle.h"


namespace eckit {


ClassSpec RadosWriteHandle::classSpec_ = {
    &DataHandle::classSpec(),
    "RadosWriteHandle",
};
Reanimator<RadosWriteHandle> RadosWriteHandle::reanimator_;

void RadosWriteHandle::print(std::ostream& s) const {
    s << "RadosWriteHandle[" << object_ << ']';
}

void RadosWriteHandle::encode(Stream& s) const {
    DataHandle::encode(s);
    s << object_;
    s << Length(0);  // For future extensio
}

RadosWriteHandle::RadosWriteHandle(Stream& s) :
    DataHandle(s), object_(s), opened_(false) {
    s >> maxObjectSize_;
    if (!maxObjectSize_) {
        maxObjectSize_ = RadosCluster::instance().maxObjectSize();
    }
}

RadosWriteHandle::RadosWriteHandle(const std::string& name, const Length& maxObjectSize) :
    object_(name), maxObjectSize_(maxObjectSize), opened_(false) {
    if (!maxObjectSize_) {
        maxObjectSize_ = RadosCluster::instance().maxObjectSize();
    }
}

RadosWriteHandle::~RadosWriteHandle() {}

Length RadosWriteHandle::openForRead() {
    NOTIMP;
}

void RadosWriteHandle::openForWrite(const Length& length) {

    ASSERT(!opened_);

    written_  = 0;
    position_ = 0;
    part_     = 0;
    opened_   = true;
}

void RadosWriteHandle::openForAppend(const Length&) {
    NOTIMP;
}

long RadosWriteHandle::read(void* buffer, long length) {

    NOTIMP;
}

long RadosWriteHandle::write(const void* buffer, long length) {

    std::cout << "RadosWriteHandle::write " << length << std::endl;
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
            ASSERT(handle_);
            handle_->close();
            handle_.reset(0);
            written_ = 0;
            continue;
        }

        std::cout << "RadosWriteHandle::write " << len << " - " << maxObjectSize_ << " - " << written_ << std::endl;

        if (!handle_.get()) {

            RadosObject object(object_, part_++);
            std::cout << "RadosWriteHandle::write open " << object << std::endl;
            handle_.reset(new RadosHandle(object));
            handle_->openForWrite(0);  // TODO: use proper size
        }


        handle_->write(buf + result, l);
        written_ += l;
        result += l;
        length -= l;
        position_ += l;
    }

    return result;
}

void RadosWriteHandle::flush() {
    // NOTIMP;
}


void RadosWriteHandle::close() {
    if (handle_.get()) {
        handle_->close();
        handle_.reset(0);
    }

    if (opened_) {

        RadosAttributes attrs;

        attrs.set("length", position_);
        attrs.set("parts", part_);
        attrs.set("maxsize", maxObjectSize_);

        RadosCluster::instance().attributes(object_, attrs);

        opened_ = false;
    }
}

void RadosWriteHandle::rewind() {
    NOTIMP;
}


Offset RadosWriteHandle::position() {
    return position_;
}

std::string RadosWriteHandle::title() const {
    return PathName::shorten(object_.str());
}

}  // namespace eckit
