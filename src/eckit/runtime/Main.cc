/*
* (C) Copyright 1996-2016 ECMWF.
*
* This software is licensed under the terms of the Apache Licence Version 2.0
* which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
* In applying this licence, ECMWF does not waive the privileges and immunities
* granted to it by virtue of its status as an intergovernmental organisation nor
* does it submit to any jurisdiction.
*/

#include <unistd.h>

#include "eckit/bases/Loader.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/os/BackTrace.h"
#include "eckit/runtime/Library.h"
#include "eckit/runtime/Main.h"
#include "eckit/system/SystemInfo.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/thread/Once.h"
#include "eckit/utils/Translator.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------
static Once<Mutex> local_mutex;
static Main* instance_ = 0;
//----------------------------------------------------------------------------------------------------------------------

Main::Main(int argc, char** argv, const char* homeenv) :
    argc_(argc),
    argv_(argv),
    taskID_(-1),
    home_("/"),
    debug_(false) {

    if (instance_) {
        std::cerr << "Attempting to create a new instance of Main()" << std::endl;
        std::cerr << BackTrace::dump() << std::endl;
        _exit(1);
    }

    instance_ = this;

    if (::getenv("DEBUG")) {
        debug_ = eckit::Translator<std::string, bool>()(::getenv("DEBUG"));
    }

    for (size_t i = 1; i < argc; i++) {
        // Old style

        if (::strcmp(argv[i], "-debug")) {
            debug_ = true;

        }

        // New style
        if (::strcmp(argv[i], "--debug")) {
            debug_ = true;

        }

        if (::strncmp(argv[i], "--debug=", 8)) {
            debug_ = eckit::Translator<std::string, bool>()(argv[i] + 8);
        }

    }

    ::srand(::getpid() + ::time(0));

    name_ = displayName_ = PathName(argv[0]).baseName(false);

    const char* home = homeenv ? ::getenv(homeenv) : 0;

    if (home) {
        home_ = home;
    } else {
        std::string execHome = eckit::system::SystemInfo::instance().executablePath().dirName().dirName();
        home_ = execHome;
    }

    Loader::callAll(&Loader::execute);

}

Main::~Main() {
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

std::string Main::name() const {
    return name_;
}

// bool Main::debug() const {
//     return debug_;
// }

void Main::terminate() {
    ::exit(0);
}

void Main::initialise(int argc, char** argv, const char* homeenv) {
    AutoLock<Mutex> lock(local_mutex);
    if (instance_ == 0) {
        new Library(argc, argv, homeenv);
    }
}


Channel* Main::createInfoChannel() const {
    return createChannel();
}

Channel* Main::createWarningChannel() const {
    return createChannel();
}

Channel* Main::createErrorChannel() const {
    return createChannel();
}

Channel* Main::createDebugChannel() const {
    return debug_ ? createChannel() : new Channel();
}


//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
