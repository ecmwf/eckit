/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <pthread.h>
#include <signal.h>

#include "eckit/config/Resource.h"
#include "eckit/container/MappedArray.h"
#include "eckit/container/SharedMemArray.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/os/BackTrace.h"
#include "eckit/runtime/Main.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/runtime/TaskInfo.h"
#include "eckit/system/SystemInfo.h"
#include "eckit/thread/AutoLock.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

Monitor::TaskArray::~TaskArray() {}

class MemoryMappedTaskArray : public Monitor::TaskArray {

    virtual void sync() { map_.sync(); }
    virtual void lock() { map_.lock(); }
    virtual void unlock() { map_.unlock(); }

    virtual iterator begin() { return map_.begin(); }
    virtual iterator end() { return map_.end(); }

    virtual const_iterator begin() const { return map_.begin(); }
    virtual const_iterator end() const { return map_.end(); }

    virtual unsigned long size() { return map_.size(); }
    virtual TaskInfo& operator[](unsigned long n) { return map_[n]; }

    MappedArray<TaskInfo> map_;

public:
    MemoryMappedTaskArray(const PathName& path, unsigned long size) :
        TaskArray(), map_(path, size) {}
};

class SharedMemoryTaskArray : public Monitor::TaskArray {

    virtual void sync() { map_.sync(); }
    virtual void lock() { map_.lock(); }
    virtual void unlock() { map_.unlock(); }

    virtual iterator begin() { return map_.begin(); }
    virtual iterator end() { return map_.end(); }

    virtual const_iterator begin() const { return map_.begin(); }
    virtual const_iterator end() const { return map_.end(); }

    virtual unsigned long size() { return map_.size(); }
    virtual TaskInfo& operator[](unsigned long n) { return map_[n]; }

    SharedMemArray<TaskInfo> map_;

public:
    SharedMemoryTaskArray(const PathName& path, const std::string& name, unsigned long size) :
        TaskArray(), map_(path, name, size) {}
};

//----------------------------------------------------------------------------------------------------------------------

static bool active_ = false;

static Monitor::TaskArray* mapArray = nullptr;
static pthread_once_t once          = PTHREAD_ONCE_INIT;

static void taskarray_init(void) {

    LocalPathName monitorPath(Resource<std::string>("monitorPath", "~/etc/monitor"));
    size_t size = Resource<size_t>("monitorSize", 1000);

    std::string monitorArrayType = Resource<std::string>("monitorArrayType", "MemoryMapped");

    // Log::info() << "monitorPath: " << monitorPath << std::endl;

    if (monitorArrayType == "MemoryMapped") {
        mapArray = new MemoryMappedTaskArray(monitorPath, size);
        return;
    }

    if (monitorArrayType == "SharedMemory") {
        std::string shmpath = eckit::system::SystemInfo::instance().userName() + "-etc-monitor";
        mapArray            = new SharedMemoryTaskArray(monitorPath, shmpath, size);
        return;
    }

    std::ostringstream oss;
    oss << "Invalid monitorArrayType : " << monitorArrayType << ", valid types are 'MemoryMapped' and 'SharedMemory'"
        << std::endl;
    throw eckit::BadParameter(oss.str(), Here());
}

Monitor::TaskArray& Monitor::tasks() {
    ASSERT(active_);
    pthread_once(&once, taskarray_init);
    return *mapArray;
}

//----------------------------------------------------------------------------------------------------------------------

template class ThreadSingleton<Monitor>;

Monitor& Monitor::instance() {
    static ThreadSingleton<Monitor> monitor;
    Monitor& m = monitor.instance();
    if (!m.ready_)
        m.init();
    return m;
}

//----------------------------------------------------------------------------------------------------------------------

Monitor::Monitor() :
    slot_(0), ready_(false), check_(false) {}

void Monitor::init() {
    if (ready_)
        return;

    if (!active_) {
        ready_ = true;
        return;
    }  // this is for processes that don't use Monitor

    if (check_)
        return;  // safely ignore double initialization
    check_ = true;

    TaskArray& a = tasks();

    AutoLock<TaskArray> lock(a);

    int slot = hash() % a.size();

    bool found = false;

    for (bool check = false; !check && !found; check = true) {
        for (size_t i = 0; i < a.size() && !found; ++i) {
            slot_ = (slot + i) % a.size();
            if (!a[slot_].busy(check)) {
                new (&a[slot_]) TaskInfo();
                found = true;
            }
        }
    }

    if (!found) {
        std::cout << "No free monitor slots" << std::endl;
        std::cerr << "No free monitor slots" << std::endl;
    }

    // No free monitor slots

    PANIC(!found);

    check_ = false;
    ready_ = true;
}

Monitor::~Monitor() {
    if (ready_ && active_) {
        TaskArray& a = tasks();
        AutoLock<TaskArray> lock(a);
        a[slot_].TaskInfo::~TaskInfo();
    }
}


bool Monitor::active() {
    return active_;
}

void Monitor::active(bool a) {
    active_ = a;
}

void Monitor::reset() {
    Monitor::shutdown();
    slot_  = 0;
    ready_ = false;
    check_ = false;
    Monitor::startup();
}

void Monitor::startup() {
    if (!ready_)
        init();
}

void Monitor::shutdown() {
    if (!active_)
        return;

    TaskArray& a = tasks();
    AutoLock<TaskArray> lock(a);

    pid_t pid = ::getpid();

    for (size_t i = 0; i < a.size(); ++i)
        if (a[i].pid() == pid)
            a[i].TaskInfo::~TaskInfo();

    // should unmap
}

unsigned long Monitor::hash() {
    return (((unsigned long)::pthread_self() << 16) | (unsigned long)::getpid());
}

TaskInfo& Monitor::task() {
    return task(slot_);
}

TaskInfo& Monitor::task(unsigned long slot) {
    if (!active_ || !ready_) {
        static TaskInfo info;
        return info;
    }

    TaskArray& a = tasks();
    return a[slot];
}

long Monitor::self() {
    if (!active_) {
        return Main::instance().taskID();
    }
    if (!ready_)
        return -1;
    return slot_;
}

void Monitor::out(char* from, char* to) {
    if (!ready_)
        return;
    task().out(from, to);
}

void Monitor::name(const std::string& s) {
    if (!ready_)
        return;
    task().name(s);
}

void Monitor::kind(const std::string& s) {
    if (!ready_)
        return;
    task().kind(s);
}

void Monitor::progress(const std::string& name, unsigned long long min, unsigned long long max) {
    if (!ready_)
        return;
    task().progressName(name);
    task().start(min, max);
}

void Monitor::progress(unsigned long long value) {
    if (!ready_)
        return;
    task().progress(value);
}

void Monitor::progress() {
    if (!ready_)
        return;
    task().done();
}

char Monitor::state(char c) {
    char x = task().state();
    task().state(c);
    return x;
}

void Monitor::status(const std::string& msg) {
    if (!ready_)
        return;
    task().status(msg);
}

std::string Monitor::status() {
    if (!ready_)
        return std::string();
    return task().status();
}

void Monitor::message(const std::string& msg) {
    if (!ready_)
        return;
    task().message(msg);
}

std::string Monitor::message() {
    if (!ready_)
        return std::string();
    return task().message();
}

void Monitor::stoppable(bool b) {
    task().stoppable(b);
}

bool Monitor::stopped() {
    return task().stopped();
}

bool Monitor::stopTriggered() {
    return task().stopTriggered();
}

void Monitor::setStopped() {
    task().setStopped();
}

void Monitor::abort() {
    task().abort();
}

void Monitor::checkAbort() {
    task().checkAbort();
}

void Monitor::parent(long p) {
    task().parent(p);
}

std::string Monitor::statusTree() {
    std::ostringstream os;
    os << status();
    int n = self();

    TaskArray& p = tasks();

    for (size_t j = 0; j < p.size(); ++j)
        if ((p[j].parent() == n) && p[j].show() && p[j].busy(true)) {
            os << "|" << p[j].status();
        }

    return os.str();
}

void Monitor::start(const std::string& app) {
    if (!active_)
        return;

    TaskArray& p = tasks();

    for (size_t j = 0; j < p.size(); ++j)
        if (p[j].busy(true) && app == p[j].application() && p[j].depth() == 0) {
            Log::warning() << app << " is already running with a pid of " << p[j].pid() << std::endl;
            Log::warning() << "Please stop it first" << std::endl;
            return;
        }

    PathName cmd = Resource<PathName>("startCmd", "~/admin/starter");

    std::string s = std::string(cmd) + " " + app;

    Log::info() << "Executing shell command: " << s << std::endl;

    SYSCALL(::system(s.c_str()));
}

void Monitor::port(int p) {
    task().port(p);
}

int Monitor::port() {
    return task().port();
}

void Monitor::host(const std::string& h) {
    task().host(h);
}

std::string Monitor::host() {
    return task().host();
}

void Monitor::taskID(const TaskID& id) {
    task().taskID(id);
}

TaskID Monitor::taskID() {
    return task().taskID();
}

void Monitor::show(bool on) {
    task().show(on);
}

int Monitor::kill(const std::string& name, int sig) {

    if (!active_)
        return 0;

    Monitor::TaskArray& info = tasks();
    pid_t me                 = ::getpid();
    int n                    = 0;

    // Name. Look for Unix process ID
    for (size_t i = 0; i < info.size(); ++i)
        if (info[i].busy(true) && info[i].application() == name) {
            pid_t pid = info[i].pid();
            if (pid == me)
                Log::info() << pid << ": Suicide avoided ;-)" << std::endl;
            else {
                if (::kill(pid, sig))
                    Log::info() << Log::syserr << std::endl;
                else {
                    Log::info() << pid << ": Killed" << std::endl;
                    n++;
                }
            }
        }

    return n;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
