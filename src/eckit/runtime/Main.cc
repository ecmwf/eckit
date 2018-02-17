/*
* (C) Copyright 1996- ECMWF.
*
* This software is licensed under the terms of the Apache Licence Version 2.0
* which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
* In applying this licence, ECMWF does not waive the privileges and immunities
* granted to it by virtue of its status as an intergovernmental organisation nor
* does it submit to any jurisdiction.
*/

#include <unistd.h>
#include <stdlib.h>

#include "eckit/bases/Loader.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/log/OStreamTarget.h"
#include "eckit/os/BackTrace.h"
#include "eckit/runtime/Library.h"
#include "eckit/runtime/Main.h"
#include "eckit/system/SystemInfo.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/thread/StaticMutex.h"
#include "eckit/utils/Translator.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

static StaticMutex local_mutex;
static Main* instance_ = 0;

//----------------------------------------------------------------------------------------------------------------------

Main::Main(int argc, char** argv, const char* homeenv) :
    taskID_(-1),
    argc_(argc),
    argv_(argv),
    home_("/"),
    debug_(false) {

    if (instance_) {
        std::cerr << "Attempting to create a new instance of Main()" << std::endl;
        std::cerr << BackTrace::dump() << std::endl;
        _exit(1);
    }


    if (::getenv("DEBUG")) {
        debug_ = eckit::Translator<std::string, bool>()(::getenv("DEBUG"));
    }

    name_ = displayName_ = PathName(argv[0]).baseName(false);

    for (size_t i = 1; i < size_t(argc); ++i) {

        // Old style
        if (::strcmp(argv[i], "-debug") == 0) {
            eckit::Log::warning() << "-debug is deprecated, please use --debug" << std::endl;
            debug_ = true;
        }

        // New style
        if (::strcmp(argv[i], "--debug") == 0) {
            debug_ = true;
        }

        // New style with variable
        const char* debug = "--debug=";
        if (::strncmp(argv[i], debug, ::strlen(debug)) == 0) {
            debug_ = eckit::Translator<std::string, bool>()(argv[i] + ::strlen(debug));
        }

        // Old style -name
        if (::strcmp(argv[i], "-name") == 0) {
            ASSERT(argc > i + 1);
            displayName_ = argv[i + 1];
            eckit::Log::warning() << "-name is deprecated, please use --display-name=" << displayName_ << std::endl;
        }

        const char* display_name = "--display-name=";
        if (::strncmp(argv[i], display_name, ::strlen(display_name)) == 0) {
            displayName_ = argv[i] + ::strlen(display_name);
        }

        const char* application_name = "--application-name=";
        if (::strncmp(argv[i], application_name, ::strlen(application_name)) == 0) {
            name_ = argv[i] + ::strlen(application_name);
        }

    }

    ::srand(::getpid() + ::time(0));

    const char* home = homeenv ? ::getenv(homeenv) : 0;

    if (home) {
        home_ = home;
    } else {
        std::string execHome = eckit::system::SystemInfo::instance().executablePath().dirName().dirName();
        home_ = execHome;
    }

    instance_ = this;

    Loader::callAll(&Loader::execute);

}

Main::~Main() {
    if (instance_ == 0) {
        std::cerr << "Attempting to delete a non-existent instance of Main()" << std::endl;
        std::cerr << BackTrace::dump() << std::endl;
        _exit(1);
    }

    instance_ = 0;
}

Main& Main::instance() {
    if (!instance_) {
        std::cerr << "Attempting to access a non-existent instance of Main()" << std::endl;
        std::cerr << BackTrace::dump() << std::endl;
        _exit(1);
    }
    return *instance_;
}


int Main::argc() const {
    return argc_;
}

std::string Main::argv(int n) const {
    ASSERT(argc_ != 0 && argv_ != 0);  // check initialized
    ASSERT(n < argc_ && n >= 0);       // check bounds
    // ASSERT(argv_[n] != 0);             // check initialized??
    return argv_[n] ? argv_[n] : "<undefined>";
}

char** Main::argv() const {
    return argv_;
}

const std::string& Main::home() const {
    return home_;
}

long Main::taskID() const {
    return taskID_;
}

void Main::taskID(long id) {
    taskID_ = id;
}

std::string Main::hostname()
{
    // RFC 1035 max length is 255 chars, whilst each label '.' separated is 63 chars
    char hostname[256] = {0,};
    SYSCALL(::gethostname(hostname, sizeof(hostname) - 1));
    return hostname;
}

const std::string& Main::name() const {
    return name_;
}

const std::string& Main::displayName() const {
    return displayName_;
}

bool Main::ready() {
    return instance_ != 0;
}

void Main::terminate() {
    ::exit(0);
}

void Main::initialise(int argc, char** argv, const char* homeenv) {
    AutoLock<StaticMutex> lock(local_mutex);
    if (instance_ == 0) {
        new Library(argc, argv, homeenv);
    }
}

bool Main::debug() const {
    return debug_;
}

LogTarget* Main::createInfoLogTarget() const {
    return createDefaultLogTarget();
}

LogTarget* Main::createWarningLogTarget() const {
    return createDefaultLogTarget();
}

LogTarget* Main::createErrorLogTarget() const {
    return createDefaultLogTarget();
}

LogTarget* Main::createDebugLogTarget() const {
    return createDefaultLogTarget();
}

LogTarget* Main::createDefaultLogTarget() const  {
    return new OStreamTarget(std::cout);
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
