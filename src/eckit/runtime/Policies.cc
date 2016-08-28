/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include <stdlib.h>

#include "eckit/log/Log.h"
#include "eckit/runtime/Main.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/runtime/Policies.h"

//-----------------------------------------------------------------------------

namespace eckit {

LocationPolicy::~LocationPolicy() {}

LoggingPolicy::~LoggingPolicy() {}

MonitoringPolicy::~MonitoringPolicy() {}

SignallingPolicy::~SignallingPolicy() {}

//-----------------------------------------------------------------------------

static char* memoryReserve;

static void end(const char* msg) {
    static bool in_end = false;

    if (in_end) {
        fprintf(stderr, "terminate called twice\n");
        _exit(1);
    }

    in_end = true;

    delete[] memoryReserve;
    memoryReserve = 0;
    Log::status() << msg << std::endl;

    try {
        throw;
    } catch (std::exception& e) {
        Log::panic() << "!!!!!!!!!!!!!!!!!! ";
        Log::panic() << msg << " with the exception:" << std::endl;
        Log::panic() << e.what() << std::endl;
        //	Exception::exceptionStack(Log::panic());
    }
    _exit(1);
    // Panic(msg);
}

//-----------------------------------------------------------------------------

void DoMonitor::start() {
    try  // activate monitor
    {
        Monitor::active(true);
        Monitor::instance().startup();
        Main::instance().self(Monitor::instance().self());
    } catch (std::exception& e) {
        Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
        Log::error() << "** Exception is re-thrown" << std::endl;
        throw;
    }
}

void DoMonitor::stop() {
    Monitor::instance().shutdown();
}

//-----------------------------------------------------------------------------

StdSignalRegist::StdSignalRegist() : regist_(false) {
    memoryReserve = new char[20 * 1024];
}

StdSignalRegist::~StdSignalRegist() {
    if (!memoryReserve) {
        delete[] memoryReserve;
        memoryReserve = 0;
    }
}

void StdSignalRegist::catch_terminate() {
    end("Terminate was called");
}

void StdSignalRegist::catch_unexpected() {
    end("Unexpected was called");
}

void StdSignalRegist::catch_new_handler() {
    delete[] memoryReserve;
    memoryReserve = 0;
    throw OutOfMemory();
}

void StdSignalRegist::regist() {
    if (regist_) return;
    nh = std::set_new_handler(StdSignalRegist::catch_new_handler);
    th = std::set_terminate(StdSignalRegist::catch_terminate);
    uh = std::set_unexpected(StdSignalRegist::catch_unexpected);
    regist_ = true;
}

void StdSignalRegist::unregist() {
    if (!regist_) return;
    if (nh) std::set_new_handler(nh);
    if (th) std::set_terminate(th);
    if (uh) std::set_unexpected(uh);
    regist_ = false;
}

//-----------------------------------------------------------------------------

} // namespace eckit
