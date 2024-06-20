/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/exception/Exceptions.h"

#include "eckit/log/Channel.h"
#include "eckit/log/ChannelBuffer.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

Channel::Channel(ChannelBuffer* buffer): std::ostream(buffer), buffer_(buffer) { }

Channel::Channel(LogTarget* target): Channel(new ChannelBuffer()) {
    ASSERT(buffer_);
    if (target) { buffer_->setTarget(target); }
}

Channel::Channel(VoidBuffer* dummy): std::ostream(nullptr), buffer_(dummy) { }

Channel::~Channel() {
    buffer_->sync();
    delete buffer_;
}

bool Channel::operator!() const {
    bool b = *this;
    return !b;
}

Channel::operator bool() const {
    return buffer_->active();
}

void Channel::setCallback(channel_callback_t cb, void* data) {
    ASSERT(cb);
    buffer_->setCallback(cb, data);
}

void Channel::addCallback(channel_callback_t cb, void* data) {
    ASSERT(cb);
    buffer_->addCallback(cb, data);
}

void Channel::setTarget(LogTarget* target) {
    ASSERT(target);
    buffer_->setTarget(target);
}

void Channel::addTarget(LogTarget* target) {
    ASSERT(target);
    buffer_->addTarget(target);
}

void Channel::setStream(std::ostream& out) {
    buffer_->setStream(out);
}

void Channel::addStream(std::ostream& out) {
    buffer_->addStream(out);
}

void Channel::setFile(const std::string& path) {
    buffer_->setFile(path);
}

void Channel::addFile(const std::string& path) {
    buffer_->addFile(path);
}

void Channel::reset() {
    buffer_->reset();
}

void Channel::print(std::ostream& s) const {
    s << "Channel(buffer=" << *buffer_ << ")";
}

void Channel::indent(const char* space) {
    buffer_->indent(space);
}

void Channel::unindent() {
    buffer_->unindent();
}

//----------------------------------------------------------------------------------------------------------------------

EmptyChannel::EmptyChannel(): Channel(new VoidBuffer()) { }

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
