/*
 * (C) Copyright 1996-2015 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/// @file   Application.h
/// @date   May 1996
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Florian Rathgeber

#ifndef eckit_Application_h
#define eckit_Application_h

#include <exception>

#include "eckit/config/Configurable.h"
#include "eckit/runtime/Task.h"

//-----------------------------------------------------------------------------

namespace eckit {

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

//=============================================================================

class Application : public Task, 
                    public Configurable {
public:

    // -- Contructors

    Application(int argc, char **argv,
                LoggingPolicy* logPolicy = new DefaultLogging(),
                MonitoringPolicy* monPolicy = new NoMonitor(),
                LocationPolicy* locPolicy = new DefaultLocations(),
                SignallingPolicy* sigPolicy = new NoSignalRegist());

    // -- Destructor

    virtual ~Application();

    static Application& instance();

    // -- Methods

    virtual void run() = 0;

    /// call this function once to make this application unique on this host
    void unique();

    // -- Overridden methods

    // From Task

    virtual void start();
    virtual void stop();
    virtual void kill();
    virtual void wait() {}
    virtual bool active() { return true; }

    std::string appName() { return name_; }
    bool running() { return running_; }

    time_t uptime();

    void terminate();

protected: // methods

    // From Configurable

    virtual void   reconfigure();
    virtual std::string name() const   { return name_; }

private: // members

    LoggingPolicy* loggingPolicy_;
    MonitoringPolicy* monitoringPolicy_;
    LocationPolicy* locationPolicy_;
    SignallingPolicy* signallingPolicy_;

    std::string name_;
    bool   running_;

    static Application* instance_;

    /// overriden from Configurable
    virtual std::string kind() const  { return "Application"; }

    friend class AppCommand;    ///< in order to call reconfigure
    friend class ResourceBase;
};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
