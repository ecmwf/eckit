/*
 * (C) Copyright 1996-2016 ECMWF.
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

Channel::Channel(LogTarget* target):
    std::ostream(new ChannelBuffer()),
    buffer_(dynamic_cast<ChannelBuffer*>(rdbuf())) {
    ASSERT( buffer_ );
    if (target) {
        buffer_->setTarget(target);
    }
}


Channel::~Channel() {
    delete buffer_;
}

bool Channel::operator !() const {
    bool b = *this;
    return  !b;
}

Channel::operator bool() const {
    return buffer_->active();
}

void Channel::setTarget(LogTarget* target) {
    ASSERT(target);
    buffer_->setTarget(target);
}

void Channel::clearTarget() {
    buffer_->clearTarget();
}

void Channel::indent(const char* space) {
    buffer_->indent(space);
}

void Channel::unindent() {
    buffer_->unindent();
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
