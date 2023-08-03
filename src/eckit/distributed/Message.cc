/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cstring>

#include "eckit/log/Log.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/ResourceUsage.h"
#include "eckit/maths/Functions.h"

#include "eckit/distributed/Message.h"
#include "eckit/distributed/Actor.h"

using eckit::Log;
using eckit::Bytes;

namespace eckit::distributed {

//----------------------------------------------------------------------------------------------------------------------

class ReadyMessage : public Message {
public:
    ReadyMessage(): Message(Actor::READY) {
        (*this) << "ready";
    }
};

const Message &Message::readyMessage() {
    static ReadyMessage message;
    return message;
}
//----------------------------------------------------------------------------------------------------------------------

class ShutdownMessage : public Message {
public:
    ShutdownMessage(): Message(Actor::SHUTDOWN) {
        (*this) << "shutdown";
    }
};

const Message &Message::shutdownMessage() {
    static ShutdownMessage message;
    return message;
}

void Message::print(std::ostream& out) const
{
    out << "Message(tag=" << tag_ << ")" << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

Message::Message(int tag, size_t size) :
    tag_(tag),
    source_(-1),
    buffer_(eckit::round(size, 1024 * 1024)),
    position_(0),
    blob_(false) {
}

Message::~Message() {
}

void Message::rewind() {
    position_ = 0;
    blob_ = false;
}

void *Message::messageData() {
    return buffer_;
}

const void *Message::messageData() const {
    return buffer_;
}

size_t Message::bufferSize() const {
    return buffer_.size();
}

size_t Message::messageSize() const {
    return position_;
}

void Message::messageReceived(int tag, int source) {
    position_ = 0;
    tag_ = tag;
    source_ = source;
}

bool Message::shutdownRequested() const {
    return tag_ == Actor::SHUTDOWN;
}

int Message::tag() const {
    return tag_;
}

int Message::source() const {
    ASSERT(source_ >= 0);
    return source_;
}

long Message::read(void *buffer, long length) {

    ASSERT(!blob_);  // We should not decode once we access the blob directly

    size_t left = buffer_.size() - position_;
    size_t size = std::min(left, size_t(length));
    ::memcpy(buffer, buffer_ + position_, size);
    position_ += size;
    return size;
}

void Message::reserve(size_t size) {
    if (buffer_.size() < size) {
        buffer_.resize(eckit::round(size, 1024 * 1024));
    }
}


long Message::write(const void *buffer, long length) {

    if (position_ + length > buffer_.size()) {
        size_t newsize = eckit::round(position_ + length, 1024 * 1024);
        buffer_.resize(newsize, true);
        Log::debug() << "Message::write() resizing buffer to " << Bytes(newsize) << std::endl;
    }

    // ASSERT(!command_.empty());

    size_t left = buffer_.size() - position_;
    size_t size = std::min(left, size_t(length));
    ::memcpy(buffer_ + position_, buffer, size);

    if (size != size_t(length)) {
        std::ostringstream oss;
        oss << "Attempt to write "
            << length
            << " bytes on message, could only write "
            << size
            << ", buffer is "
            << buffer_.size();
        throw eckit::SeriousBug(oss.str());
    }


    position_ += size;
    return size;
}


const void* Message::getBlob(size_t& size) {
    size = blobSize();  // After that we should not read
    blob_ = true;
    return buffer_ + position_;
}

std::string Message::name() const {
    return "Message";
}
//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

