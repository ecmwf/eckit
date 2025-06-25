/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


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
