/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "TransportHandle.h"

#include "eckit/maths/Functions.h"
#include "eckit/exception/Exceptions.h"

#include "eckit/distributed/Transport.h"
#include "eckit/distributed/Message.h"

namespace eckit::distributed {

//----------------------------------------------------------------------------------------------------------------------

TransportHandle::TransportHandle(Transport& transport,
                                 const std::string& path,
                                 size_t writers,
                                 bool exclusive):
    transport_(transport),
    path_(path),
    writer_(0),
    opened_(false),
    append_(false),
    position_(0) {

    // Compute hash to select writer
    for (auto j = path.begin(); j != path.end(); ++j) {
        writer_ +=  (*j - 'A') + (writer_ << 5);
    }

    writer_ %= writers;
    writer_++;

    // eckit::Log::info() << "New " << *this << std::endl;
}

TransportHandle::~TransportHandle() {
    // eckit::Log::info() << "Delete " << *this << std::endl;

    close();
}

eckit::Length TransportHandle::openForRead() {
    NOTIMP;
}

void TransportHandle::openForWrite(const eckit::Length& length) {
    append_ = false;

    Message message(Actor::OPEN, eckit::round(path_.length()  + 64, 1024));
    message << path_;
    message << append_;

    transport_.sendToWriter(writer_, message);
    opened_ = true;
}

void TransportHandle::openForAppend(const eckit::Length& length) {
    append_ = true;

    Message message(Actor::OPEN, eckit::round(path_.length()  + 64, 1024));
    message << path_;
    message << append_;

    transport_.sendToWriter(writer_, message);
    opened_ = true;
}

long TransportHandle::read(void* buffer, long length) {
    NOTIMP;
}

long TransportHandle::write(const void* buffer, long length) {

    ASSERT(opened_);

    position_ += length;
    send(buffer, length);
    return length;
}

void TransportHandle::send(const void *buffer, size_t length) {

    if (!length) {
        return;
    }

    Message message(Actor::WRITE, eckit::round(length + path_.length()  + 64, 1024 * 1024));
    message << path_;
    message << append_;
    message.writeBlob(buffer, length);

    transport_.sendToWriter(writer_, message);

}

void TransportHandle::close() {
    if (opened_) {
        flush();


        Message message(Actor::CLOSE, eckit::round(path_.length()  + 64, 1024));
        message << path_;
        message << append_;

        transport_.sendToWriter(writer_, message);
    }
    opened_ = false;
}

void TransportHandle::flush() {

}

void TransportHandle::rewind() {
    NOTIMP;
}

void TransportHandle::print(std::ostream& s) const {
    s << "TransportHandle[transport=" << transport_;
    s << ",path=" << path_;
    s << ",write=" << writer_;
    s << ']';
}

eckit::Length TransportHandle::estimate() {
    return 0;
}

eckit::Offset TransportHandle::position() {
    return position_;
}

std::string TransportHandle::title() const {
    return std::string("TransportHandle[") + eckit::PathName::shorten(path_) + "]";
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
