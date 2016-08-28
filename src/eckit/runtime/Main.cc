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
    home_("/"),
    assertAborts_(false),
    debugLevel_(0) {

    ::srand(::getpid() + ::time(0));

    name_ = displayName_ = PathName(argv[0]).baseName(false);

    abortHandler_ = &(::abort);


    info_.setLogTarget(new OStreamTarget(std::cout));
    warning_.setLogTarget(new OStreamTarget(std::cout));
    error_.setLogTarget(new OStreamTarget(std::cout));


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

void Main::abortHandler(abort_handler_t h) {
    AutoLock<Mutex> lock(local_mutex);
    abortHandler_ = h;
}

void Main::abort()
{
    abortHandler_();
}

void Main::assertAborts(bool assertAborts) {
    AutoLock<Mutex> lock(local_mutex);
    assertAborts_ = assertAborts;
}

bool Main::assertAborts() {
    AutoLock<Mutex> lock(local_mutex);
    return assertAborts_;
}

long Main::taskID() const {
    return taskID_;
}

void Main::taskID(long id) {
    taskID_ = id;
}

int Main::debugLevel() const { return debugLevel_; }

void Main::debugLevel(int level) { debugLevel_ = level;}



const std::string& Main::displayName() const {
    return displayName_;
}


std::string Main::name() const {
    return name_;
}


Channel& Main::infoChannel() { return info_; }

Channel& Main::warnChannel() { return warning_; }

Channel& Main::errorChannel() { return error_; }


void Main::reconfigure()
{
    Log::info() << "Tool::reconfigure" << std::endl;

    int debug = Resource<int>(this, "debug;$DEBUG;-debug", 0);

    Main::instance().debugLevel( debug );

    // forward to context
    Main::instance().reconfigure();
}

Channel& Main::channel(const std::string& key)
{
    AutoLock<Mutex> lock(local_mutex);

    ChannelRegistry::iterator itr = channels_.find(key);

    if (itr != channels_.end()) { return *itr->second; }

    if       ( key == "Error" )  { return errorChannel(); }
    else if  ( key == "Warn"  )  { return warnChannel(); }
    else if  ( key == "Info"  )  { return infoChannel(); }

    // requested channel not found
    throw BadParameter( "Channel '" + key + "' not found ", Here());
}

void Main::registerChannel(const std::string& key, Channel* channel)
{
    AutoLock<Mutex> lock(local_mutex);

    if (channels_.find(key) == channels_.end()) {
        channels_.insert(std::make_pair(key, channel));
    }
    else {
        throw BadParameter( "Channel '" + key + "' is already registered ", Here());
    }
}

void Main::removeChannel(const std::string& key)
{
    AutoLock<Mutex> lock(local_mutex);

    if (channels_.find(key) == channels_.end()) {
        throw BadParameter( "Channel '" + key + "' does not exist ", Here());
    }
    else {
        channels_.erase(key);
    }
}

static PathName proc_path(const std::string& name) {
    ASSERT(name.size() > 0);
    if (name[0] == '/') {
        return PathName(name);
    }

    if (name[0] == '.') {
        char buf[MAXPATHLEN];
        ASSERT(getcwd(buf, sizeof(buf)));
        return PathName(std::string(buf) + "/" + name);
    }

    char* path = getenv("PATH");
    ASSERT(path);

    Tokenizer parse(":");
    std::vector<std::string> v;

    parse(path, v);
    for (size_t i = 0; i < v.size(); i++) {
        PathName p(v[i] + "/" + name);
        if (p.exists()) {
            return proc_path(p);
        }
    }
    throw SeriousBug("Cannot find " + name + " in PATH");
}

PathName Main::commandPath() const { return proc_path(argv(0)); }



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
