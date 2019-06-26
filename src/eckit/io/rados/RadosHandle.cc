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

RadosHandle::RadosHandle(Stream& s):
    DataHandle(s),
    object_(s),
    offset_(0),
    opened_(false),
    write_(false) {
}

RadosHandle::RadosHandle(const RadosObject& object):
    object_(object),
    offset_(0),
    opened_(false),
    write_(false)
{}


RadosHandle::RadosHandle(const std::string& object):
    object_(object),
    offset_(0),
    opened_(false),
    write_(false)
{}

RadosHandle::~RadosHandle() {
    if (opened_) {
        close();
    }
}

void RadosHandle::open() {


    std::cout << "RadosHandle::open " << object_ << std::endl;


    ASSERT(!opened_);


    offset_ = 0;

    RADOS_CALL(rados_ioctx_create(RadosCluster::instance().cluster(),
                                  object_.pool().c_str(),
                                  &io_ctx_));

    opened_ = true;
}

Length RadosHandle::openForRead() {


    std::cout << "RadosHandle::openForRead " << object_ << std::endl;


    open();
    write_ = false;

    // rados_xattrs_iter_t iter;
    // RADOS_CALL(rados_getxattrs(io_ctx_, object_.oid().c_str(), &iter));


    // for (;;) {
    //     const char *name;

    //     const char* val;
    //     size_t len;

    //     RADOS_CALL(rados_getxattrs_next(iter, &name, &val, &len));
    //     if (!name) {
    //         break;
    //     }
    //     std::cout << "rados_getxattrs_next " << name << " = " << val << " (" << len << ")" << std::endl;
    // }

    // rados_getxattrs_end(iter);

    uint64_t psize;
    time_t pmtime;

    RADOS_CALL(rados_stat(io_ctx_, object_.oid().c_str(), &psize, &pmtime));

    return psize;
}

void RadosHandle::openForWrite(const Length& length) {


    std::cout << "RadosHandle::openForWrite " << object_ << " " << length << std::endl;


    open();
    write_ = true;
}

void RadosHandle::openForAppend(const Length&) {
    NOTIMP;
}

long RadosHandle::read(void* buffer, long length) {

    std::cout << "RadosHandle::read " << object_ << " " << length << std::endl;

    ASSERT(opened_);
    ASSERT(!write_);

    int len = RADOS_CALL(rados_read(io_ctx_,
                                    object_.oid().c_str(),
                                    reinterpret_cast<char*>(buffer),
                                    length,
                                    offset_));
    ASSERT(len  > 0);

    offset_ += len;

    return len;
}

long RadosHandle::write(const void* buffer, long length) {
    std::cout << "RadosHandle::write " << object_ << " " << length << std::endl;

    ASSERT(opened_);
    ASSERT(write_);



    RADOS_CALL(rados_write(io_ctx_,
                           object_.oid().c_str(),
                           reinterpret_cast<const char*>(buffer),
                           length,
                           offset_));

    offset_ += length;

    return length;


}

void RadosHandle::flush() {
    // NOTIMP;
}


void RadosHandle::close() {


    std::cout << "RadosHandle::close " << object_ << std::endl;


    ASSERT(opened_);
    std::cout << "RADOS_CALL => rados_ioctx_destroy(io_ctx_)" << std::endl;
    rados_ioctx_destroy(io_ctx_);
    opened_ = false;
    std::cout << "RADOS_CALL <= rados_ioctx_destroy(io_ctx_)" << std::endl;

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
