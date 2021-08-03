/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include <unistd.h>
#include <fstream>
#include <iostream>

#include "eckit/bases/Loader.h"
#include "eckit/config/Resource.h"
#include "eckit/log/TimeStampTarget.h"
#include "eckit/os/Semaphore.h"
#include "eckit/runtime/Application.h"
#include "eckit/runtime/Monitor.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

static char* reserve_ = nullptr;

static void end(const char* msg) {
    static bool in_end = false;

    if (in_end) {
        std::cout << "terminate called twice" << std::endl;
        _exit(1);
    }

    in_end = true;

    delete[] reserve_;
    reserve_ = nullptr;

    try {
        throw;
    }
    catch (std::exception& e) {
        std::cout << "!!!!!!!!!!!!!!!!!! ";
        std::cout << msg << " with the exception:" << std::endl;
        std::cout << e.what() << std::endl;
    }
    _exit(1);
}

static void catch_terminate() {
    end("Terminate was called");
}

static void catch_unexpected() {
    end("Unexpected was called");
}

[[noreturn]] static void catch_new_handler() {
    delete[] reserve_;
    reserve_ = nullptr;
    throw eckit::OutOfMemory();
}

//----------------------------------------------------------------------------------------------------------------------

Application& Application::instance() {
    return static_cast<Application&>(Main::instance());
}

Application::Application(int argc, char** argv, const char* homeenv) :
    Main(argc, argv, homeenv), running_(false) {
    reserve_ = new char[20 * 1024];  // reserve in case we runout of memory

    std::set_new_handler(&catch_new_handler);
    std::set_terminate(&catch_terminate);
    std::set_unexpected(&catch_unexpected);

    Monitor::active(true);
    Monitor::instance().startup();
    taskID_ = Monitor::instance().self();

    Loader::callAll(&Loader::execute);
}


Application::~Application() {
    Monitor::instance().shutdown();
}

LogTarget* Application::createInfoLogTarget() const {
    return new TimeStampTarget("(I)");
}

LogTarget* Application::createWarningLogTarget() const {
    return new TimeStampTarget("(W)");
}

LogTarget* Application::createErrorLogTarget() const {
    return new TimeStampTarget("(E)");
}

LogTarget* Application::createDebugLogTarget() const {
    return new TimeStampTarget("(D)");
}

void Application::start() {

    int status = 0;

    Log::info() << "** Start of " << displayName() << " ** pid is " << getpid() << std::endl;

    try {
        Log::status() << "Running" << std::endl;
        running_ = true;
        run();
        running_ = false;
    }
    catch (std::exception& e) {
        status = 1;
        Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
        Log::error() << "** Exception terminates " << displayName() << std::endl;
    }

    Log::info() << "** End of " << displayName() << " (" << argv(0) << ")  **" << std::endl;

    ::exit(status);
}


void Application::stop() {
    ::exit(0);  // or throw Stop();
}


void Application::kill() {
    ::exit(1);
}


void Application::terminate() {
    stop();
}


void Application::unique() {

    PathName lockFile("~/locks/" + name_);

    if (!lockFile.exists())
        lockFile.touch();

    Semaphore* sem = new Semaphore(lockFile);

    if (sem->test()) {
        std::ifstream os(lockFile.localPath());
        std::string s;
        os >> s;
        throw SeriousBug("Application " + name_ + " is already running as pid " + s);
    }

    sem->lock();  // OS should reset the semaphore

    std::ofstream os(lockFile.localPath());
    os << ::getpid();
}

time_t Application::uptime() {

    TaskInfo& info = Monitor::instance().task(taskID_);
    time_t uptime  = info.start();
    time_t now     = ::time(0);

    return now - uptime;
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
