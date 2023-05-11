/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/message/Splitter.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/PeekHandle.h"
#include "eckit/message/Message.h"

#include <algorithm>
#include <iomanip>

namespace eckit::message {

Splitter::Splitter(eckit::PeekHandle& handle) :
    handle_(handle) {}

Splitter::~Splitter() {}


//----------------------------------------------------------------------------------------------------------------------


SplitterFactory& SplitterFactory::instance() {
    static SplitterFactory theinstance;
    return theinstance;
}

void SplitterFactory::enregister(SplitterBuilderBase* b) {
    std::lock_guard<std::mutex> lock(mutex_);
    decoders_.push_back(b);
}

void SplitterFactory::deregister(const SplitterBuilderBase* b) {
    std::lock_guard<std::mutex> lock(mutex_);
    decoders_.erase(std::remove(decoders_.begin(), decoders_.end(), b), decoders_.end());
}


SplitterBuilderBase::SplitterBuilderBase() {
    SplitterFactory::instance().enregister(this);
}

SplitterBuilderBase::~SplitterBuilderBase() {
    SplitterFactory::instance().deregister(this);
}

Splitter* SplitterFactory::lookup(eckit::PeekHandle& handle) {
    std::lock_guard<std::mutex> lock(mutex_);

    size_t n = decoders_.size();
    ASSERT(n > 0);

    for (size_t i = 0; i < n; ++i) {
        SplitterBuilderBase* builder = decoders_[(i + index_) % n];
        if (builder->match(handle)) {
            index_ = i;  // Start with this index for next message
            return builder->make(handle);
        }
    }

    std::ostringstream oss;
    oss << "Cannot find a metkit SplitterBuilder for " << handle << " ";

    for (size_t i = 0; i < handle.peeked(); ++i) {
        unsigned char c = handle.peek(i);
        oss << (isprint(c) ? char(c) : '.');
    }

    oss << " - ";
    for (size_t i = 0; i < handle.peeked(); ++i) {
        unsigned char c = handle.peek(i);
        oss << std::setfill('0') << std::setw(2) << std::hex << int(c);
    }

    oss << std::endl;
    throw eckit::SeriousBug(oss.str());
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::message
