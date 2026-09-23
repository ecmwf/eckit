// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/message/Decoder.h"

#include <algorithm>
#include <string>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/message/Message.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"

namespace eckit::message {

//----------------------------------------------------------------------------------------------------------------------

namespace {
eckit::Mutex* local_mutex              = nullptr;
std::vector<MessageDecoder*>* decoders = nullptr;
pthread_once_t once                    = PTHREAD_ONCE_INIT;
void init() {
    local_mutex = new eckit::Mutex();
    decoders    = new std::vector<MessageDecoder*>();
}

size_t index = 0;

}  // namespace


MessageDecoder::MessageDecoder() {
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);
    decoders->push_back(this);
}

MessageDecoder::~MessageDecoder() {
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);
    decoders->erase(std::remove(decoders->begin(), decoders->end(), this), decoders->end());
}

MessageDecoder& MessageDecoder::lookup(const Message& msg) {
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    size_t n = decoders->size();
    ASSERT(n);

    for (size_t i = 0; i < n; ++i) {
        MessageDecoder* d = (*decoders)[(i + index) % n];
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

}  // namespace eckit::message
