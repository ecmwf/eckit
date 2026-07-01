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

// ClassSpec RadosHandle::classSpec_ = {
//     &DataHandle::classSpec(),
//     "RadosHandle",
// };

// Reanimator<RadosHandle> RadosHandle::reanimator_;

void RadosHandle::print(std::ostream& s) const {
    s << "RadosHandle[" << object_.str() << ']';
}

// void RadosHandle::encode(Stream& s) const {
//     DataHandle::encode(s);
//     s << object_;
// }

// RadosHandle::RadosHandle(Stream& s) :
//     DataHandle(s), object_(s), offset_(0), opened_(false), write_(false) {}

RadosHandle::RadosHandle(const RadosObject& object) :
    object_(object), offset_(0), opened_(false), write_(false), first_write_(false) {}

// RadosHandle::RadosHandle(const std::string& object) :
//     object_(object), offset_(0), opened_(false), write_(false) {}

RadosHandle::~RadosHandle() {

    if (opened_) eckit::Log::error() << "RadosHandle not closed before destruction." << std::endl;

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

    write_ = true;
    first_write_ = true;

}

// void RadosHandle::openForAppend(const Length&) {
//     NOTIMP;
// }

long RadosHandle::read(void* buffer, long length) {

    ASSERT(opened_);
    ASSERT(!write_);

    /// @note: is this useful at all? even if trimming down the length, if offset_ > 0 
    ///   then the read call will request a range that exceed the object span.
    /// @note: is this check/trimming desirable? in the read pathway, we want to discover the length of the parts
    ///   rather than assuming a certain configured length which might have had a different value on write.
    // long maxLength = RadosCluster::instance().maxObjectSize();
    // long readLength = length > maxLength ? maxLength : length;

    int len = RADOS_CALL(
        rados_read(
            RadosCluster::instance().ioCtx(object_), 
            object_.name().c_str(),
            reinterpret_cast<char*>(buffer),
            length, offset_
        )
    );

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

        RADOS_CALL(
            rados_write_full(
                RadosCluster::instance().ioCtx(object_), 
                object_.name().c_str(),
                reinterpret_cast<const char*>(buffer), 
                length
            )
        );

        first_write_ = false;

    } else {

        RADOS_CALL(
            rados_write(
                RadosCluster::instance().ioCtx(object_), 
                object_.name().c_str(),
                reinterpret_cast<const char*>(buffer), 
                length, offset_
            )
        );

    }

    offset_ += length;

    return length;

}

void RadosHandle::flush() {
    // NOOP
}

eckit::Offset RadosHandle::seek(const eckit::Offset& offset) {
    offset_ = offset;
    /// @todo: assert offset <= size() ?
    return offset_;
}

void RadosHandle::close() {

    ASSERT(opened_);

    opened_ = false;

}

// void RadosHandle::rewind() {
//     offset_ = 0;
// }


Offset RadosHandle::position() {
    return offset_;
}

// std::string RadosHandle::title() const {
//     return PathName::shorten(object_.str());
// }

}  // namespace eckit
