  /*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <string>
#include <cstdarg>
#include <sys/param.h>

#include "Context.h"

#include "eckit/eckit_config.h"
#include "eckit/log/Log.h"
#include "eckit/os/BackTrace.h"
#include "eckit/runtime/ContextBehavior.h"
#include "eckit/runtime/StandardBehavior.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/thread/Mutex.h"
#include "eckit/thread/Once.h"
#include "eckit/thread/ThreadSingleton.h"
#include "eckit/parser/Tokenizer.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

static Once<Mutex> local_mutex;

Context::Context() : argc_(0), argv_(0), taskID_(0), home_("/"), runName_("undef"), displayName_() {

  char* h = getenv("HOME");
  if (h) home_ = h;

  char* th = getenv("_TEST_ECKIT_HOME");
  if (th) home_ = th;

  behavior_.reset(new StandardBehavior());
}

Context::~Context() {}

Context& Context::instance() {
  AutoLock<Mutex> lock(local_mutex);
  static Context ctxt;
  return ctxt;
}

void Context::setup(int argc, char** argv) {
  if (argc <= 0 || argv == 0)
    throw SeriousBug("Context setup with bad command line arguments");

  argc_ = argc;
  argv_ = argv;
}

void Context::behavior(ContextBehavior* b) {
  AutoLock<Mutex> lock(local_mutex);
  if (!b) throw SeriousBug("Context setup with bad context behavior");
  behavior_.reset(b);
}

ContextBehavior& Context::behavior() const {
  AutoLock<Mutex> lock(local_mutex);
  return *behavior_.get();
}

int Context::debug() const { return behavior_->debug(); }

void Context::debug(const int d) { behavior_->debug(d); }

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

char** Context::argvs() const { return argv_; }

void Context::reconfigure() { behavior_->reconfigure(); }

const std::string& Context::home() const {
  AutoLock<Mutex> lock(local_mutex);
  return home_;
}

void Context::home(const std::string& h) {
  AutoLock<Mutex> lock(local_mutex);
  home_ = h;
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

Channel& Context::infoChannel() { return behavior_->infoChannel(); }

Channel& Context::warnChannel() { return behavior_->warnChannel(); }

Channel& Context::errorChannel() { return behavior_->errorChannel(); }

Channel& Context::debugChannel() { return behavior_->debugChannel(); }

Channel& Context::channel(int cat) { return behavior_->channel(cat); }

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

static RegisterConfigHome* config_dirs = 0;

RegisterConfigHome::RegisterConfigHome(const char* name, size_t ndirs, const char* dir, ...)
    : next_(config_dirs),
      first_(true),
      name_(name),
      dirs_(ndirs) {

    ASSERT(ndirs>0);
    va_list dirs;
    va_start(dirs, ndirs);
    dirs_[0] = dir;
    for (unsigned i=1; i<ndirs; ++i) {
        dirs_[i] = va_arg(dirs, const char*);
    }
    va_end(dirs);

    config_dirs = this;
}

PathName Context::configHome(bool& first, const std::vector< const char* >& dirs) const {

    // Determine run-time command realpath
    // (catches PathName::RealName possible exception)
    std::string command_path;
    try {
        const std::string command_realpath = commandPath().realName().asString();
        command_path = command_realpath;
    }
    catch (FailedSystemCall&) {
        throw SeriousBug("No available home: run-time command path not found");
    }
    ASSERT(command_path.length());

    // Determine the available directories
    // (catches PathName::RealName possible exception)
    std::vector< std::string > available_dirs;
    for (std::vector< const char* >::const_iterator p=dirs.begin(); p!=dirs.end(); ++p) {
        try {
            PathName path = PathName(*p).realName();
            available_dirs.push_back(path.asString());
        }
        catch (FailedSystemCall&) {
        }
    }
    if (!available_dirs.size()) {
        throw SeriousBug("No available home: no available directories available");
    }
    ASSERT(available_dirs.size());

    // Home path selection, in order:
    // 1. if Context::setup(argc,argv) not called, first available path
    // 2. first available directory containing run-time command path
    // 3. first available directory
    std::string home(argc_==0? available_dirs.front():"");

    for (std::vector< std::string >::const_iterator p=available_dirs.begin(); p!=available_dirs.end() && !home.length(); ++p) {
        if (command_path.find(*p) == 0) {
            home = *p;
        }
    }

    if (!home.length()) {
        home = available_dirs.front();
    }
    ASSERT(home.length());

    if (first) {
        Log::warning()
                << "Context::configHome selected: \"" << home << "\"\n"
                << "  commandPath:      \"" << command_path << "\"\n"
                << "  setup(argc,argv)? " << (argc_==0? "not called":"called")
                << std::endl;
        first = false;
    }

    return home;
}

PathName Context::configHome(const std::string& name) const {
  RegisterConfigHome* d = config_dirs;
  while (d) {
    if (name == d->name_) {
      return configHome(d->first_, d->dirs_);
    }
    d = d->next_;
  }

  throw SeriousBug("Not registered HOME for ~" + name);
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
