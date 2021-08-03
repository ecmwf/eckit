/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/os/SignalHandler.h"
#include "eckit/log/Log.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

SignalHandler* SignalHandler::current_ = 0;

SignalHandler::SignalHandler(void (*handler)(int), SignalHandler::Signal sig) :
    signal_(sig), next_(current_) {
    //	Log::info() << "Installing signal handler " << signal_ << std::endl;
    current_ = this;

    struct sigaction a;

    a.sa_flags   = 0;
    a.sa_handler = handler;
    sigemptyset(&a.sa_mask);

    // volatile int sigtype = sigsetjmp(buf_,1);
    // if(sigtype == 0)
    ::sigaction(signal_, &a, &save_);
    // else {
    // Log::warning() << "Got signal " << sigtype << std::endl;
    // throw Abort("Signal received");
    //}
}

SignalHandler::~SignalHandler() {
    // ASSERT(current_ == this);
    current_ = next_;

    struct sigaction ignore;
    ::sigaction(signal_, &save_, &ignore);

    //	Log::info() << "Removing signal handler " << signal_ << std::endl;
}

static bool interrupted_ = false;

void SignalHandler::interrupt(int sig) {
    //    printf(" >>> received interrupt %d", sig );

    if (interrupted_) {
        ::kill(0, SIGTERM);
    }
    // siglongjmp(current_->buf_,sig);
    interrupted_ = true;
}

void SignalHandler::checkInterrupt() {
    //    printf(".\n");

    if (interrupted_ && !Exception::throwing()) {
        interrupted_ = false;
        throw Cancel("Process interrupted");
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
