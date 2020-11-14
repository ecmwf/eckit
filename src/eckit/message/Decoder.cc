/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/message/Decoder.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/message/Message.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"

#include <algorithm>
#include <string>
#include <vector>

namespace eckit {
namespace message {

//----------------------------------------------------------------------------------------------------------------------

namespace {
eckit::Mutex* local_mutex       = 0;
std::vector<Decoder*>* decoders = 0;
pthread_once_t once             = PTHREAD_ONCE_INIT;
void init() {
    local_mutex = new eckit::Mutex();
    decoders    = new std::vector<Decoder*>();
}

size_t index = 0;

}  // namespace


Decoder::Decoder() {
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);
    decoders->push_back(this);
}

Decoder::~Decoder() {
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);
    decoders->erase(std::remove(decoders->begin(), decoders->end(), this), decoders->end());
}

Decoder& Decoder::lookup(const Message& msg) {
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    size_t n = decoders->size();
    ASSERT(n);

    for (size_t i = 0; i < n; ++i) {
        Decoder* d = (*decoders)[(i + index) % n];
        if (d->match(msg)) {
            index = i;  // Start with this index for next message
            return *d;
        }
    }

    std::ostringstream oss;
    oss << "Cannot find a Decoder for " << msg << std::endl;
    throw eckit::SeriousBug(oss.str());
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace message
}  // namespace eckit
