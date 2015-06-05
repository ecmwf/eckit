/*
 * (C) Copyright 1996-2015 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/// @file   Policies.h
/// @date   June 2015
/// @author Florian Rathgeber

#ifndef eckit_Policies_h
#define eckit_Policies_h

#include <exception>
#include <new>

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class Stream;

//-----------------------------------------------------------------------------

// Abstract policy base classes

struct LocationPolicy {
    virtual void setup() = 0;
};

struct LoggingPolicy {
    virtual void setup() = 0;
};

struct MonitoringPolicy {
    virtual void start() = 0;
    virtual void stop() = 0;
};

struct SignallingPolicy {
    virtual void regist() = 0;
    virtual void unregist() = 0;
};

//-----------------------------------------------------------------------------

struct DefaultLogging : LoggingPolicy {
    void setup() {}
};

//-----------------------------------------------------------------------------

struct DefaultLocations : LocationPolicy {
    void setup() {}
};

//-----------------------------------------------------------------------------

struct NoMonitor : MonitoringPolicy {
    void start() {}
    void stop() {}
};

struct DoMonitor : MonitoringPolicy {
    void start();
    void stop();
};

//-----------------------------------------------------------------------------

struct NoSignalRegist : SignallingPolicy {
    void regist() {}
    void unregist() {}
};

//-----------------------------------------------------------------------------

struct StdSignalRegist : SignallingPolicy {
    StdSignalRegist();
    ~StdSignalRegist();
    
    static void catch_terminate();
    static void catch_unexpected();
    static void catch_new_handler();
    
    bool regist_;
    
    std::new_handler nh;
    std::terminate_handler  th;
    std::unexpected_handler uh;

    void regist();
    void unregist();
};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
