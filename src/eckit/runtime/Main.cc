/*
* (C) Copyright 1996-2016 ECMWF.
*
* This software is licensed under the terms of the Apache Licence Version 2.0
* which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
* In applying this licence, ECMWF does not waive the privileges and immunities
* granted to it by virtue of its status as an intergovernmental organisation nor
* does it submit to any jurisdiction.
*/

#include <sys/param.h>
#include <unistd.h>

#include "eckit/parser/Tokenizer.h"
#include "eckit/thread/Mutex.h"
#include "eckit/thread/Once.h"
#include "eckit/log/OStreamTarget.h"
#include "eckit/filesystem/PathName.h"

#include "eckit/bases/Loader.h"
#include "eckit/config/Resource.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/log/Log.h"
#include "eckit/runtime/Main.h"
#include "eckit/system/SystemInfo.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------
static Once<Mutex> local_mutex;
static Main* instance_ = 0;
//----------------------------------------------------------------------------------------------------------------------

Main::Main(int argc, char** argv, const char* homeenv) :
    argc_(argc),
    argv_(argv),
    taskID_(-1),
    home_("/"){

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
    PANIC(instance_ == 0);
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

void Main::terminate() {
    ::exit(0);
}

void Main::initialise(int argc, char** argv, const char* homeenv) {
    AutoLock<Mutex> lock(local_mutex);
    if (instance_ == 0) {
        new Main(argc, argv, homeenv);
    }
}


//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
