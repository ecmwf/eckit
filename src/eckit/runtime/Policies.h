/*
 * (C) Copyright 1996-2016 ECMWF.
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

class LocationPolicy {
public:
    virtual ~LocationPolicy();

    virtual void setup() = 0;
};

class LoggingPolicy {
public:
    virtual ~LoggingPolicy();

    virtual void setup() = 0;
};

class MonitoringPolicy {
public:
    virtual ~MonitoringPolicy();

    virtual void start() = 0;
    virtual void stop() = 0;
};

class SignallingPolicy {
public:
    virtual ~SignallingPolicy();

    virtual void regist() = 0;
    virtual void unregist() = 0;
};

//-----------------------------------------------------------------------------

class DefaultLogging : public LoggingPolicy {
public:
    virtual void setup() {}
};

//-----------------------------------------------------------------------------

class DefaultLocations : public LocationPolicy {
public:
    virtual void setup() {}
};

//-----------------------------------------------------------------------------

class NoMonitor : public MonitoringPolicy {
public:
    virtual void start() {}
    virtual void stop() {}
};

class DoMonitor : public MonitoringPolicy {
public:
    virtual void start();
    virtual void stop();
};

//-----------------------------------------------------------------------------

class NoSignalRegist : public SignallingPolicy {
public:
    virtual void regist() {}
    virtual void unregist() {}
};

//-----------------------------------------------------------------------------

class StdSignalRegist : public SignallingPolicy {
public:
    StdSignalRegist();
    virtual ~StdSignalRegist();

    static void catch_terminate();
    static void catch_unexpected();
    static void catch_new_handler();

    virtual void regist();
    virtual void unregist();

private:
    bool regist_;

    std::new_handler nh;
    std::terminate_handler  th;
    std::unexpected_handler uh;
};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
