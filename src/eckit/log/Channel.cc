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

OutputChannel::OutputChannel(LogTarget* target):
    Channel(new ChannelBuffer()), buffer_(dynamic_cast<ChannelBuffer*>(rdbuf())) {
    ASSERT(buffer_);
    if (target) { buffer_->setTarget(target); }
}

OutputChannel::~OutputChannel() {
    buffer_->sync();
    delete buffer_;
}

bool OutputChannel::operator!() const {
    bool b = *this;
    return !b;
}

OutputChannel::operator bool() const {
    return buffer_->active();
}

void OutputChannel::setCallback(channel_callback_t cb, void* data) {
    ASSERT(cb);
    buffer_->setCallback(cb, data);
}

void OutputChannel::addCallback(channel_callback_t cb, void* data) {
    ASSERT(cb);
    buffer_->addCallback(cb, data);
}

void OutputChannel::setTarget(LogTarget* target) {
    ASSERT(target);
    buffer_->setTarget(target);
}

void OutputChannel::addTarget(LogTarget* target) {
    ASSERT(target);
    buffer_->addTarget(target);
}

void OutputChannel::setStream(std::ostream& out) {
    buffer_->setStream(out);
}

void OutputChannel::addStream(std::ostream& out) {
    buffer_->addStream(out);
}

void OutputChannel::setFile(const std::string& path) {
    buffer_->setFile(path);
}

void OutputChannel::addFile(const std::string& path) {
    buffer_->addFile(path);
}

void OutputChannel::reset() {
    buffer_->reset();
}

void OutputChannel::print(std::ostream& s) const {
    s << "OutputChannel(buffer=" << *buffer_ << ")";
}

void OutputChannel::indent(const char* space) {
    buffer_->indent(space);
}

void OutputChannel::unindent() {
    buffer_->unindent();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
