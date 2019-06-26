/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/rados/RadosWriteHandle.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosHandle.h"

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
}

RadosWriteHandle::RadosWriteHandle(Stream& s):
    DataHandle(s),
    object_(s) {
}

RadosWriteHandle::RadosWriteHandle(const std::string& name, const Length& maxObjectSize):
    object_(name)
{
    if (!maxObjectSize_) {
        maxObjectSize_ = RadosCluster::instance().maxObjectSize();
    }
}

RadosWriteHandle::~RadosWriteHandle() {

}

Length RadosWriteHandle::openForRead() {
    NOTIMP;
}

void RadosWriteHandle::openForWrite(const Length& length) {
    written_ = 0;
    position_ = 0;
    part_ = 0;
}

void RadosWriteHandle::openForAppend(const Length&) {
    NOTIMP;
}

long RadosWriteHandle::read(void* buffer, long length) {

    NOTIMP;
}

long RadosWriteHandle::write(const void* buffer, long length) {

    long result = 0;
    const char* buf = reinterpret_cast<const char*>(buffer);

    while (length > 0) {

        Length len = std::min(Length(maxObjectSize_ - Length(written_)), Length(length));

        long l     = (long)len;

        ASSERT(len == Length(l));

        if (!handle_.get()) {

            RadosObject object(object_, part_++);


            std::cout << "RadosWriteHandle::write open " << object << std::endl;
            handle_.reset(new RadosHandle(object));
            handle_->openForWrite(0); // TODO: use proper size
        }


        handle_->write(buf + result, l);
        written_ += len;
        result += len;
        length -= len;

        ASSERT(Length(written_) <= maxObjectSize_);

        if (Length(written_) == maxObjectSize_) {
            handle_->close();
            handle_.reset(0);
        }

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
