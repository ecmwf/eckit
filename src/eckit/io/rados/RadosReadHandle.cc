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

namespace eckit {


ClassSpec RadosReadHandle::classSpec_ = {
    &DataHandle::classSpec(),
    "RadosReadHandle",
};
Reanimator<RadosReadHandle> RadosReadHandle::reanimator_;

void RadosReadHandle::print(std::ostream& s) const {
    s << "RadosReadHandle[file=" << name_ << ']';
}

void RadosReadHandle::encode(Stream& s) const {
    DataHandle::encode(s);
    s << name_;
}

RadosReadHandle::RadosReadHandle(Stream& s):
    DataHandle(s),
    offset_(0),
    opened_(false),
    write_(false) {
    s >> name_;
}

RadosReadHandle::RadosReadHandle(const std::string& name):
    name_(name),
    offset_(0),
    opened_(false),
    write_(false)
{}

RadosReadHandle::~RadosReadHandle() {

}

Length RadosReadHandle::openForRead() {
    ASSERT(!opened_);

    ASSERT(!cluster_);
    cluster_.reset(new RadosCluster());

    offset_ = 0;

    RADOS_CALL(rados_ioctx_create(cluster_->cluster(), "fdb", &io_ctx_));
    opened_ = true;
    write_ = false;

    return 0;
}

void RadosReadHandle::openForWrite(const Length& length) {

    ASSERT(!opened_);

    ASSERT(!cluster_);
    cluster_.reset(new RadosCluster());

    ASSERT(length > 0);
    offset_ = 0;

    RADOS_CALL(rados_ioctx_create(cluster_->cluster(), "fdb", &io_ctx_));
    opened_ = true;
    write_ = true;

}

void RadosReadHandle::openForAppend(const Length&) {
    NOTIMP;
}

long RadosReadHandle::read(void* buffer, long length) {

    ASSERT(opened_);
    ASSERT(!write_);

    int len = RADOS_CALL(rados_read(io_ctx_, name_.c_str(), reinterpret_cast<char*>(buffer), length, offset_));
    ASSERT(len  > 0);

    offset_ += len;

    return len;
}

long RadosReadHandle::write(const void* buffer, long length) {

    ASSERT(opened_);
    ASSERT(write_);

    RADOS_CALL(rados_write(io_ctx_, name_.c_str(), reinterpret_cast<const char*>(buffer), length, offset_));

    offset_ += length;

    return length;


}

void RadosReadHandle::flush() {
    NOTIMP;
}


void RadosReadHandle::close() {
    ASSERT(opened_);
    rados_ioctx_destroy(io_ctx_);
    opened_ = false;
}

void RadosReadHandle::rewind() {
    NOTIMP;
}


Offset RadosReadHandle::position() {
    NOTIMP;
}

std::string RadosReadHandle::title() const {
    return PathName::shorten(name_);
}

}  // namespace eckit
