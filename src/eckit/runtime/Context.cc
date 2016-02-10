  /*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "Context.h"

#include <sys/param.h>
#include <string>
#include <vector>

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

RegisterConfigHome::RegisterConfigHome(const char* name, const char* install_bin_dir, const char* developer_bin_dir,
                                       const char* install_config_dir, const char* developer_config_dir)
    : next_(config_dirs),
      first_(true),
      name_(name),
      install_bin_dir_(install_bin_dir),
      developer_bin_dir_(developer_bin_dir),
      install_config_dir_(install_config_dir),
      developer_config_dir_(developer_config_dir) {
  config_dirs = this;
}

PathName Context::configHome(bool& first,
                             const char* install_bin_dir,               // From ecbuild : APPNAME_INSTALL_BIN_DIR
                             const char* developer_bin_dir,             // From ecbuild : APPNAME_DEVELOPER_BIN_DIR
                             const char* install_config_dir,            // From ecbuild : APPNAME_INSTALL_DATA_DIR
                             const char* developer_config_dir) const {  // From ecbuild : APPNAME_DEVELOPER_BIN_DIR

  if (argc_ == 0) {   // Context was not initialised

    if (first) {
      Log::warning() << "Context::setup(argc, argv) was not called, assuming running from " << install_bin_dir
                     << std::endl;
      Log::warning() << "Using development configuration path " << install_config_dir << std::endl;
      first = false;
    }
    return install_config_dir;
  }

  std::string path = eckit::PathName(commandPath()).realName().asString();
  std::string install_bin_dir_path = eckit::PathName(install_bin_dir).realName().asString();

  if (path.find(install_bin_dir_path) == 0) {
    return install_config_dir;
  }

  std::string developer_bin_dir_path = eckit::PathName(developer_bin_dir).realName().asString();

  if (path.find(developer_bin_dir_path) == 0) {
    if (first) {
      Log::warning() << "Current command is from " << developer_bin_dir << std::endl;
      Log::warning() << "Using development configuration path " << developer_config_dir << std::endl;
      first = false;
    }
    return developer_config_dir;
  }

  if (first) {
    Log::warning() << "Current command is " << path << ", and is neither in " << install_bin_dir << " or "
                   << developer_config_dir << std::endl;
    Log::warning() << "Assuming configuration files are in " << install_config_dir << std::endl;
    first = false;
  }

  return install_config_dir;
}

PathName Context::configHome(const std::string& name) const {
  RegisterConfigHome* d = config_dirs;
  while (d) {
    if (name == d->name_) {
      return configHome(d->first_, d->install_bin_dir_, d->developer_bin_dir_, d->install_config_dir_,
                        d->developer_config_dir_);
    }
    d = d->next_;
  }

  throw SeriousBug("Not registered HOME for ~" + name);
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
