/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <signal.h>
#include <sys/time.h>
#include <unistd.h>

#include <iterator>

#include "eckit/log/JSON.h"
#include "eckit/log/Timer.h"
#include "eckit/memory/Zero.h"
#include "eckit/os/SignalHandler.h"
#include "eckit/runtime/Main.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/runtime/ProcessControler.h"
#include "eckit/runtime/TaskInfo.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

TaskInfo::TaskInfo() {
    ASSERT(busy_ == false);

    eckit::zero(*this);

    pid_    = getpid();
    thread_ = pthread_self();
    pos_    = 0;
    start_  = ::time(nullptr);
    if (Main::ready()) {
        ::strncpy(name_, Main::instance().displayName().c_str(), sizeof(name_) - 1);
        ::strncpy(kind_, Main::instance().name().c_str(), sizeof(kind_) - 1);
        ::strncpy(application_, Main::instance().name().c_str(), sizeof(application_) - 1);
        ::strcpy(status_, "Starting");
    }
    show_ = true;
    start(0, 0);
    busy_      = true;
    stoppable_ = true;
    parent_    = -1;
    state_     = ' ';
}


TaskInfo::~TaskInfo() {
    busy_ = false;
}

void TaskInfo::kind(const std::string& s) {
    touch();
    strncpy(kind_, s.c_str(), sizeof(kind_) - 1);
}

void TaskInfo::name(const std::string& s) {
    touch();
    strncpy(name_, s.c_str(), sizeof(name_) - 1);
}

void TaskInfo::status(const std::string& s) {
    touch();
    strncpy(status_, s.c_str(), sizeof(status_) - 1);
}

void TaskInfo::message(const std::string& s) {
    touch();
    zero(message_);
    strncpy(message_, s.c_str(), sizeof(message_) - 1);
}

void TaskInfo::progressName(const std::string& s) {
    touch();
    strncpy(progress_.name_, s.c_str(), sizeof(progress_.name_) - 1);
}

void TaskInfo::out(char* from, char* to) {
    touch();
    for (char* p = from; p != to; p++)
        buffer_[(pos_++) % size_] = *p;
}

unsigned long TaskInfo::text(char* buf, unsigned long max, unsigned long& pos) const {
    unsigned long len = pos_ - pos;
    if (len > size_)
        len = size_;
    if (len > max)
        len = max;

    unsigned long p = pos_ - len + size_;
    unsigned long n = len;

    while (n-- > 0)
        *buf++ = buffer_[(p++) % size_];

    pos = pos_;

    return len;
}

bool TaskInfo::busy(bool check) {
    if (!busy_)
        return false;

    time_t now = ::time(nullptr);

    // After 2 minutes, force the check
    if ((now - check_) > 120)
        check = true;

    check_ = now;

    if (!check)
        return busy_;

    // Check first

    if (!ProcessControler::isRunning(pid_)) {
        this->TaskInfo::~TaskInfo();
        return false;
    }

    return true;
}

void TaskInfo::start(unsigned long long min, unsigned long long max) {
    progress_.rate_ = progress_.speed_ = 0;
    progress_.min_                     = min;
    progress_.max_                     = max;
    progress_.val_                     = min;
    ::gettimeofday(&progress_.start_, nullptr);
    ::gettimeofday(&progress_.last_, nullptr);

    touch();
}

void TaskInfo::progress(unsigned long long val) {
    ::timeval now;
    ::gettimeofday(&now, nullptr);

    ::timeval diff = now - progress_.last_;

    double elapsed = (static_cast<double>(diff.tv_sec) + (static_cast<double>(diff.tv_usec) / 1000000.));
    if (elapsed > 0) {
        progress_.rate_ = (val - progress_.val_) / elapsed;
    }

    diff = now - progress_.start_;

    elapsed = (static_cast<double>(diff.tv_sec) + (static_cast<double>(diff.tv_usec) / 1000000.));
    if (elapsed > 0) {
        progress_.speed_ = (val - progress_.min_) / elapsed;
    }

    progress_.val_ = val;

    ::gettimeofday(&progress_.last_, nullptr);
    touch();
}

void TaskInfo::done() {
    start(0, 0);
}

void TaskInfo::touch() {
    checkAbort();

    // FIXME: potential race condition (reported by Clang ThreadSanitizer)
    check_ = last_ = ::time(nullptr);
    busy_          = true;

    SignalHandler::checkInterrupt();

    if (stop_ && stoppable_) {
        stopped_ = true;
        stop_    = false;
        exit(1);
    }
}

void TaskInfo::checkAbort() {
    if (abort_) {
        abort_ = false;
        throw Abort("ThreadControler aborted by request");
    }
}

void TaskInfo::parent(long p) {
    parent_ = p;
    depth_  = 0;
    if (p >= 0) {
        depth_ = Monitor::instance().task(static_cast<unsigned long>(p)).depth() + 1;
    }
}

//----------------------------------------------------------------------------------------------------------------------

void TaskInfo::json(JSON& json) const {

    Monitor& monitor = Monitor::instance();
    auto n           = std::distance(monitor.tasks().cbegin(), this);

    json.startObject();

    json << "id" << n;
    json << "busy" << busy_;
    // json << "thread" << thread_;
    json << "pid" << pid_;
    json << "start" << start_;
    json << "last" << last_;
    json << "check" << check_;
    json << "show" << show_;
    json << "late" << late_;


    // Logging
    // enum { size_ = 10240 };
    // char           buffer_[size_];
    // unsigned long  pos_;

    char buffer[size_ + 1] = {};

    unsigned long pos{0};
    text(buffer, sizeof(buffer), pos);

    json << "log" << buffer;

    json << "name" << name_;
    json << "kind" << kind_;
    json << "application" << application_;

    json << "progress";
    json.startObject();

    json << "min" << progress_.min_;
    json << "max" << progress_.max_;
    json << "val" << progress_.val_;
    json << "name" << progress_.name_;
    json << "rate" << progress_.rate_;
    json << "speed" << progress_.speed_;

    json.endObject();

    json << "taskID" << taskID_;

    json << "stop" << stop_;
    json << "abort" << abort_;
    json << "stoppable" << stoppable_;
    json << "stopped" << stopped_;
    json << "canceled" << canceled_;
    json << "exception" << exception_;
    json << "cancelMsg" << cancelMsg_;
    json << "config" << config_;
    // json << "resource" << resource_;
    json << "parent" << parent_;
    json << "depth" << depth_;
    json << "state" << state_;
    json << "port" << port_;
    json << "host" << host_;
    json << "message" << message_;

    json.endObject();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
