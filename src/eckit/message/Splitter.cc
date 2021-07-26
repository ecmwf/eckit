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
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"

#include <algorithm>
#include <iomanip>
#include <string>
#include <vector>

namespace eckit {
namespace message {


Splitter::Splitter(eckit::PeekHandle& handle) :
    handle_(handle) {}

Splitter::~Splitter() {}


//----------------------------------------------------------------------------------------------------------------------

namespace {
eckit::Mutex* local_mutex               = 0;
std::vector<SplitterFactory*>* decoders = 0;
pthread_once_t once                     = PTHREAD_ONCE_INIT;
void init() {
    local_mutex = new eckit::Mutex();
    decoders    = new std::vector<SplitterFactory*>();
}

size_t index = 0;

}  // namespace


SplitterFactory::SplitterFactory() {
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);
    decoders->push_back(this);
}

SplitterFactory::~SplitterFactory() {
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);
    decoders->erase(std::remove(decoders->begin(), decoders->end(), this), decoders->end());
}

Splitter* SplitterFactory::lookup(eckit::PeekHandle& handle) {
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    size_t n = decoders->size();
    ASSERT(n);

    for (size_t i = 0; i < n; ++i) {
        SplitterFactory* d = (*decoders)[(i + index) % n];
        if (d->match(handle)) {
            index = i;  // Start with this index for next message
            return d->make(handle);
        }
    }

    std::ostringstream oss;
    oss << "Cannot find a metkit SplitterFactory for " << handle << " ";

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

}  // namespace message
}  // namespace eckit
