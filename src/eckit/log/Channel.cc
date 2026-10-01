// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/exception/Exceptions.h"

#include "eckit/log/Channel.h"
#include "eckit/log/ChannelBuffer.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

Channel::Channel(LogTarget* target) :
    std::ostream(new ChannelBuffer()), buffer_(dynamic_cast<ChannelBuffer*>(rdbuf())) {
    ASSERT(buffer_);
    if (target) {
        buffer_->setTarget(target);
    }
    //    std::cerr << "Channel::Channel()" << std::endl;
}


Channel::~Channel() {
    //    std::cerr << "Channel::~Channel() " << *this << std::endl;
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

}  // namespace eckit
