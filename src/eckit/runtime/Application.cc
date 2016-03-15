/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/bases/Loader.h"
#include "eckit/config/Resource.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/log/Log.h"
#include "eckit/os/Semaphore.h"
#include "eckit/runtime/Application.h"
#include "eckit/runtime/Context.h"
#include "eckit/runtime/Monitor.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

Application::Application(int argc, char** argv,
                         LoggingPolicy* logPolicy,
                         MonitoringPolicy* monPolicy,
                         LocationPolicy* locPolicy,
                         SignallingPolicy* sigPolicy)
    : loggingPolicy_(logPolicy),
      monitoringPolicy_(monPolicy),
      locationPolicy_(locPolicy),
      signallingPolicy_(sigPolicy),
      running_(false)
{
    locationPolicy_->setup();  // location policy first -- sets home()

    name_ = PathName(argv[0]).baseName(false);

    Context& ctxt = Context::instance();
    ctxt.setup(argc, argv);
    ctxt.runName(name_);
    ctxt.debug(Resource<int>(this, "debug;$DEBUG;-debug", 0));

    loggingPolicy_->setup();

    monitoringPolicy_->start();

    ::srand(::getpid() + ::time(0));

    // ensure is uniqe instance of this application
    {
        if (instance_) throw SeriousBug("An instance of application already exists");
        instance_ = this;
    }

    signallingPolicy_->regist();

    Loader::callAll(&Loader::execute);
}

//-----------------------------------------------------------------------------

Application::~Application() {
    signallingPolicy_->unregist();
    monitoringPolicy_->stop();
    instance_ = 0;
}

//-----------------------------------------------------------------------------

Application& Application::instance() {
    PANIC(instance_ == 0);
    return *instance_;
}

//-----------------------------------------------------------------------------

void Application::reconfigure() {
    Log::info() << "Application::reconfigure" << std::endl;

    int debug = Resource<int>(this, "debug;$DEBUG;-debug", 0);

    Context::instance().debug(debug);

    Context::instance().reconfigure();  // forward to context
}

//-----------------------------------------------------------------------------

void Application::start() {
    int status = 0;

    std::string displayName = Resource<std::string>("-name", name_);

    Context::instance().displayName(displayName);

    Monitor::instance().name(displayName);

    Log::info() << "** Start of " << name() << " ** pid is " << getpid() << std::endl;

    try {
        Log::status() << "Running" << std::endl;
        running_ = true;
        run();
        running_ = false;
    } catch (std::exception& e) {
        status = 1;
        Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
        Log::error() << "** Exception is terminates " << name() << std::endl;
    }

    Log::info() << "** End of " << name() << " (" << Context::instance().argv(0) << ")  **" << std::endl;

    ::exit(status);
}

//-----------------------------------------------------------------------------

void Application::stop() {
    ::exit(0);  // or: throw Stop();
}

//-----------------------------------------------------------------------------

void Application::kill() {
    ::exit(1);
}

//-----------------------------------------------------------------------------

void Application::terminate() {
    stop();
}

//-----------------------------------------------------------------------------

void Application::unique() {
    PathName lockFile("~/locks/" + name());

    if (!lockFile.exists()) lockFile.touch();

    Semaphore* sem = new Semaphore(lockFile);

    if (sem->test()) {
        std::ifstream os(lockFile.localPath());
        std::string s;
        os >> s;
        throw SeriousBug("Application " + name() + " is already running as pid " + s);
    }

    sem->lock();  // OS should reset the semaphore

    std::ofstream os(lockFile.localPath());
    os << getpid();
}

//-----------------------------------------------------------------------------

time_t Application::uptime() {
    long taskId = Context::instance().self();

    Monitor::TaskArray& info = Monitor::instance().tasks();
    time_t uptime = info[taskId].start();
    time_t now = ::time(0);

    return now - uptime;
}

//-----------------------------------------------------------------------------

Application* Application::instance_;

//-----------------------------------------------------------------------------

} // namespace eckit
