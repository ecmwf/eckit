// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/message/Splitter.h"

#include <algorithm>
#include <iomanip>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/PeekHandle.h"
#include "eckit/message/Message.h"

namespace eckit::message {

Splitter::Splitter(eckit::PeekHandle& handle) : handle_(handle) {}

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
