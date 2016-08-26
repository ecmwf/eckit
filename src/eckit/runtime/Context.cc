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

#include "Context.h"

#include "eckit/parser/Tokenizer.h"
#include "eckit/thread/Mutex.h"
#include "eckit/thread/Once.h"
#include "eckit/log/OStreamTarget.h"
#include "eckit/filesystem/PathName.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

static Context* context = 0;
static pthread_once_t once = PTHREAD_ONCE_INIT;

static Once<Mutex> local_mutex;

void Context::init() {
    context = new Context();
}

//----------------------------------------------------------------------------------------------------------------------

Context::Context() :
    argc_(0),
    argv_(0),
    taskID_(-1),
    home_("/"),
    runName_("undef"),
    displayName_(),
    assertAborts_(false) {
    char* h = getenv("HOME");
    if (h) home_ = h;

    char* th = getenv("_TEST_ECKIT_HOME");
    if (th) home_ = th;

    abortHandler_ = &(::abort);


    info_.setLogTarget(new OStreamTarget(std::cout));
    warning_.setLogTarget(new OStreamTarget(std::cout));
    error_.setLogTarget(new OStreamTarget(std::cout));

}

Context::~Context() {}

Context& Context::instance() {
    pthread_once(&once, Context::init);
    return *context;
}

void Context::setup(int argc, char** argv) {
    AutoLock<Mutex> lock(local_mutex);

    if (argc <= 0 || argv == 0) { throw SeriousBug("Context setup with bad command line arguments"); }

    argc_ = argc;
    argv_ = argv;
}


int Context::argc() const {
    AutoLock<Mutex> lock(local_mutex);
    return argc_;
}

std::string Context::argv(int n) const {
    AutoLock<Mutex> lock(local_mutex);
    ASSERT(argc_ != 0 && argv_ != 0);  // check initialized
    ASSERT(n < argc_ && n >= 0);       // check bounds
    // ASSERT(argv_[n] != 0);             // check initialized??
    return argv_[n] ? argv_[n] : "<undefined>";
}

char** Context::argvs() const {
    AutoLock<Mutex> lock(local_mutex);
    return argv_;
}

void Context::reconfigure() {
    AutoLock<Mutex> lock(local_mutex);
    // behavior_->reconfigure();
}

const std::string& Context::home() const {
    AutoLock<Mutex> lock(local_mutex);
    return home_;
}

void Context::home(const std::string& h) {
    AutoLock<Mutex> lock(local_mutex);
    home_ = h;
}

void Context::abortHandler(abort_handler_t h) {
    AutoLock<Mutex> lock(local_mutex);
    abortHandler_ = h;
}

void Context::abort()
{
    abortHandler_();
}

void Context::assertAborts(bool assertAborts) {
    AutoLock<Mutex> lock(local_mutex);
    assertAborts_ = assertAborts;
}

bool Context::assertAborts() {
    AutoLock<Mutex> lock(local_mutex);
    return assertAborts_;
}

long Context::self() const { return taskID_; }

void Context::self(long id) { taskID_ = id; }

const std::string& Context::runName() const {
    AutoLock<Mutex> lock(local_mutex);
    return runName_;
}

void Context::runName(const std::string& name) {
    AutoLock<Mutex> lock(local_mutex);
    runName_ = name;
}

const std::string& Context::displayName() const {
    AutoLock<Mutex> lock(local_mutex);
    if (displayName_.empty())
        return runName();
    else
        return displayName_;
}

void Context::displayName(const std::string& name) {
    AutoLock<Mutex> lock(local_mutex);
    displayName_ = name;
}

Channel& Context::infoChannel() { return info_; }

Channel& Context::warnChannel() { return warning_; }

Channel& Context::errorChannel() { return error_; }



Channel& Context::channel(const std::string& key)
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

void Context::registerChannel(const std::string& key, Channel* channel)
{
    AutoLock<Mutex> lock(local_mutex);

    if (channels_.find(key) == channels_.end()) {
        channels_.insert(std::make_pair(key, channel));
    }
    else {
        throw BadParameter( "Channel '" + key + "' is already registered ", Here());
    }
}

void Context::removeChannel(const std::string& key)
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

PathName Context::commandPath() const { return proc_path(argv(0)); }

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
