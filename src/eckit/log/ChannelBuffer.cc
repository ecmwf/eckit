/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/CallbackTarget.h"
#include "eckit/log/ChannelBuffer.h"
#include "eckit/log/FileTarget.h"
#include "eckit/log/IndentTarget.h"
#include "eckit/log/LogTarget.h"
#include "eckit/log/OStreamTarget.h"
#include "eckit/log/TeeTarget.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ChannelBuffer::ChannelBuffer(std::size_t size) :
    std::streambuf(), target_(0), buffer_(size) {
    ASSERT(size);
    char* base = &buffer_.front();
    setp(base, base + buffer_.size());
}

ChannelBuffer::~ChannelBuffer() {
    reset();
}

bool ChannelBuffer::active() const {
    return target_ != 0;
}


void ChannelBuffer::setTarget(LogTarget* target) {
    ASSERT(target);

    sync();

    target->attach();

    if (target_) {
        target_->detach();
    }

    target_ = target;
}


void ChannelBuffer::addTarget(LogTarget* target) {
    ASSERT(target);
    setTarget(new TeeTarget(target_, target));
}

void ChannelBuffer::reset() {
    sync();
    if (target_) {
        target_->detach();
        target_ = 0;
    }
}

bool ChannelBuffer::dumpBuffer() {
    if (target_) {
        target_->write(pbase(), pptr());
    }
    setp(pbase(), epptr());
    return true;
}

void ChannelBuffer::indent(const char* space) {
    if (target_) {
        setTarget(new IndentTarget(space, target_));
    }
}

void ChannelBuffer::unindent() {
    if (target_) {
        IndentTarget* indent = dynamic_cast<IndentTarget*>(target_);
        if (indent == 0) {
            throw SeriousBug("Attempt to unindent a Channel that is not indented");
        }
        setTarget(indent->target_);
    }
}

void ChannelBuffer::addCallback(channel_callback_t cb, void* data) {
    setTarget(new TeeTarget(target_, new CallbackTarget(cb, data)));
}

void ChannelBuffer::setCallback(channel_callback_t cb, void* data) {
    setTarget(new CallbackTarget(cb, data));
}

void ChannelBuffer::addStream(std::ostream& out) {
    setTarget(new TeeTarget(target_, new OStreamTarget(out)));
}

void ChannelBuffer::setStream(std::ostream& out) {
    setTarget(new OStreamTarget(out));
}

void ChannelBuffer::addFile(const std::string& path, size_t bufferSize) {
    setTarget(new TeeTarget(target_, new FileTarget(path, bufferSize)));
}

void ChannelBuffer::setFile(const std::string& path, size_t bufferSize) {
    setTarget(new FileTarget(path, bufferSize));
}

std::streambuf::int_type ChannelBuffer::overflow(std::streambuf::int_type ch) {
    if (ch == traits_type::eof()) {
        return sync();
    }
    dumpBuffer();
    sputc(ch);
    return traits_type::to_int_type(ch);
}

std::streambuf::int_type ChannelBuffer::sync() {
    if (dumpBuffer()) {
        if (target_) {
            target_->flush();
        }
        return 0;
    }
    else
        return -1;
}

void ChannelBuffer::print(std::ostream& s) const {
    s << "ChannelBuffer(size=" << buffer_.size();
    if (target_) {
        s << ", target=" << *target_;
    }
    s << ")";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
