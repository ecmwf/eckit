// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/thread/Thread.h"

#include "eckit/thread/AutoLock.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


Thread::Thread(bool autodel) : stop_(false), autodel_(autodel) {}

Thread::~Thread() {}

void Thread::stop() {
    AutoLock<Mutex> lock(mutex_);
    stop_ = true;
}

bool Thread::stopped() {
    AutoLock<Mutex> lock(mutex_);
    return stop_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
