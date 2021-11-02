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
#include <cstdlib>
#include <cstring>
#include <string>

#include "eckit/bases/Loader.h"
#include "eckit/config/Resource.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/log/OStreamTarget.h"
#include "eckit/os/BackTrace.h"
#include "eckit/runtime/Library.h"
#include "eckit/runtime/Main.h"
#include "eckit/system/LibraryManager.h"
#include "eckit/system/SystemInfo.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/thread/StaticMutex.h"
#include "eckit/utils/Translator.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

static StaticMutex local_mutex;
static Main* instance_ = nullptr;

//----------------------------------------------------------------------------------------------------------------------

Main::Main(int argc, char** argv, const char* homeenv) :
    taskID_(-1), argc_(argc), argv_(argv), home_("/"), debug_(false) {

    if (instance_) {
        std::cerr << "Attempting to create a new instance of Main()" << std::endl;
        std::cerr << BackTrace::dump() << std::endl;
        _exit(1);
    }


    if (::getenv("MAIN_DEBUG")) {
        debug_ = eckit::Translator<std::string, bool>()(::getenv("MAIN_DEBUG"));
    }

    name_ = displayName_ = PathName(argv[0]).baseName(false);

    for (int i = 1; i < argc; ++i) {

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
    ::srandom(::getpid() + ::time(0));

    const char* home = homeenv ? ::getenv(homeenv) : nullptr;

    if (home) {
        home_ = home;
    }
    else {
        std::string execHome = eckit::system::SystemInfo::instance().executablePath().dirName().dirName();
        home_                = execHome;
    }

    // n.b. instance_ must be set before call to eckit::Resource

    instance_ = this;

    // Load the libraries configured to be dynamically loaded at runtime
    // This may include eckit::Plugin libraries, but they must be identified by the filename (libname) not Plugin name
    // Note this also works for non eckit::Plugin libraries
    std::vector<std::string> libraries = Resource<std::vector<std::string>>("dynamicLibraries", {});
    for (const std::string& library : libraries) {
        void* h = system::LibraryManager::loadLibrary(library);
        if (not h) {
            std::ostringstream ss;
            ss << "Library " << library << " not found";
            Log::error() << ss.str() << std::endl;
            throw SeriousBug(ss.str(), Here());
        }
    }

    // Load eckit::Plugin libraries
    std::vector<std::string> plugins = Resource<std::vector<std::string>>("$LOAD_PLUGINS;loadPlugins", {});
    bool autoLoadPlugins             = Resource<bool>("$AUTO_LOAD_PLUGINS;autoLoadPlugins;-autoLoadPlugins", true);
    if (autoLoadPlugins or plugins.size()) {
        Log::debug() << "Configured to load plugins " << plugins << std::endl;
        system::LibraryManager::autoLoadPlugins(plugins);
    }

    Log::debug() << "Application " << name_ << " loaded libraries: " << system::LibraryManager::list() << std::endl;

    Loader::callAll(&Loader::execute);
}

Main::~Main() {
    if (not instance_) {
        std::cerr << "Attempting to delete a non-existent instance of Main()" << std::endl;
        std::cerr << BackTrace::dump() << std::endl;
        _exit(1);
    }

    instance_ = nullptr;
}

Main& Main::instance() {
    if (not instance_) {
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
    ASSERT(argc_ != 0 && argv_ != nullptr);  // check initialized
    ASSERT(n < argc_ && n >= 0);             // check bounds
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

std::string Main::hostname() {
    // RFC 1035 max length is 255 chars, whilst each label '.' separated is 63 chars
    char hostname[256] = {};
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
    return instance_ != nullptr;
}

void Main::terminate() {
    ::exit(0);
}

void Main::initialise(int argc, char** argv, const char* homeenv) {
    AutoLock<StaticMutex> lock(local_mutex);
    if (not instance_) {
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

LogTarget* Main::createMetricsLogTarget() const {
    return createDefaultLogTarget();
}

LogTarget* Main::createDefaultLogTarget() const {
    return new OStreamTarget(std::cout);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
