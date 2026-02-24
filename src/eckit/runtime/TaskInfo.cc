/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <sys/time.h>
#include <unistd.h>
#include <csignal>

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
    ASSERT(info.busy_ == false);

    info.pid_    = getpid();
    info.thread_ = pthread_self();
    info.pos_    = 0;
    info.start_  = ::time(nullptr);
    if (Main::ready()) {
        ::strncpy(info.name_, Main::instance().displayName().c_str(), sizeof(info.name_) - 1);
        ::strncpy(info.kind_, Main::instance().name().c_str(), sizeof(info.kind_) - 1);
        ::strncpy(info.application_, Main::instance().name().c_str(), sizeof(info.application_) - 1);
        ::strcpy(info.status_, "Starting");
    }
    info.show_ = true;
    start(0, 0);
    info.busy_      = true;
    info.stoppable_ = true;
    info.parent_    = -1;
    info.state_     = ' ';
}


TaskInfo::~TaskInfo() {
    info.busy_ = false;
}

void TaskInfo::kind(const std::string& s) {
    touch();
    strncpy(info.kind_, s.c_str(), sizeof(info.kind_) - 1);
}

void TaskInfo::name(const std::string& s) {
    touch();
    strncpy(info.name_, s.c_str(), sizeof(info.name_) - 1);
}

void TaskInfo::status(const std::string& s) {
    touch();
    strncpy(info.status_, s.c_str(), sizeof(info.status_) - 1);
}

void TaskInfo::message(const std::string& s) {
    touch();
    zero(info.message_);
    strncpy(info.message_, s.c_str(), sizeof(info.message_) - 1);
}

void TaskInfo::progressName(const std::string& s) {
    touch();
    strncpy(info.progress_.name_, s.c_str(), sizeof(info.progress_.name_) - 1);
}

void TaskInfo::out(char* from, char* to) {
    touch();
    for (char* p = from; p != to; p++) {
        info.buffer_[(info.pos_++) % info.size_] = *p;
    }
}

unsigned long TaskInfo::text(char* buf, unsigned long max, unsigned long& pos) const {
    unsigned long len = info.pos_ - pos;
    if (len > info.size_) {
        len = info.size_;
    }
    if (len > max) {
        len = max;
    }

    unsigned long p = info.pos_ - len + info.size_;
    unsigned long n = len;

    while (n-- > 0) {
        *buf++ = info.buffer_[(p++) % info.size_];
    }

    pos = info.pos_;

    return len;
}

bool TaskInfo::busy(bool check) {
    if (!info.busy_) {
        return false;
    }

    time_t now = ::time(nullptr);

    // After 2 minutes, force the check
    if ((now - info.check_) > 120) {
        check = true;
    }

    info.check_ = now;

    if (!check) {
        return info.busy_;
    }

    // Check first

    if (!ProcessControler::isRunning(info.pid_)) {
        this->TaskInfo::~TaskInfo();
        return false;
    }

    return true;
}

void TaskInfo::start(unsigned long long min, unsigned long long max) {
    info.progress_.rate_ = info.progress_.speed_ = 0;
    info.progress_.min_                          = min;
    info.progress_.max_                          = max;
    info.progress_.val_                          = min;
    ::gettimeofday(&info.progress_.start_, nullptr);
    ::gettimeofday(&info.progress_.last_, nullptr);

    touch();
}

void TaskInfo::progress(unsigned long long val) {
    ::timeval now;
    ::gettimeofday(&now, nullptr);

    ::timeval diff = now - info.progress_.last_;

    double elapsed = (static_cast<double>(diff.tv_sec) + (static_cast<double>(diff.tv_usec) / 1000000.));
    if (elapsed > 0) {
        info.progress_.rate_ = (val - info.progress_.val_) / elapsed;
    }

    diff = now - info.progress_.start_;

    elapsed = (static_cast<double>(diff.tv_sec) + (static_cast<double>(diff.tv_usec) / 1000000.));
    if (elapsed > 0) {
        info.progress_.speed_ = (val - info.progress_.min_) / elapsed;
    }

    info.progress_.val_ = val;

    ::gettimeofday(&info.progress_.last_, nullptr);
    touch();
}

void TaskInfo::done() {
    start(0, 0);
}

void TaskInfo::touch() {
    checkAbort();

    // FIXME: potential race condition (reported by Clang ThreadSanitizer)
    info.check_ = info.last_ = ::time(nullptr);
    info.busy_               = true;

    SignalHandler::checkInterrupt();

    if (info.stop_ && info.stoppable_) {
        info.stopped_ = true;
        info.stop_    = false;
        exit(1);
    }
}

void TaskInfo::checkAbort() {
    if (info.abort_) {
        info.abort_ = false;
        throw Abort("ThreadControler aborted by request");
    }
}

void TaskInfo::parent(long p) {
    info.parent_ = p;
    info.depth_  = 0;
    if (p >= 0) {
        info.depth_ = Monitor::instance().task(static_cast<unsigned long>(p)).depth() + 1;
    }
}

//----------------------------------------------------------------------------------------------------------------------

void TaskInfo::json(JSON& json) const {

    Monitor& monitor = Monitor::instance();
    auto n           = std::distance(monitor.tasks().cbegin(), this);

    json.startObject();

    json << "id" << n;
    json << "busy" << info.busy_;
    // json << "thread" << thread_;
    json << "pid" << info.pid_;
    json << "start" << info.start_;
    json << "last" << info.last_;
    json << "check" << info.check_;
    json << "show" << info.show_;
    json << "late" << info.late_;


    // Logging
    // enum { size_ = 10240 };
    // char           buffer_[size_];
    // unsigned long  pos_;

    char buffer[Info::size_ + 1] = {};

    unsigned long pos{0};
    text(buffer, sizeof(buffer), pos);

    json << "log" << buffer;

    json << "name" << info.name_;
    json << "kind" << info.kind_;
    json << "application" << info.application_;

    json << "progress";
    json.startObject();

    json << "min" << info.progress_.min_;
    json << "max" << info.progress_.max_;
    json << "val" << info.progress_.val_;
    json << "name" << info.progress_.name_;
    json << "rate" << info.progress_.rate_;
    json << "speed" << info.progress_.speed_;

    json.endObject();

    json << "taskID" << info.taskID_;

    json << "stop" << info.stop_;
    json << "abort" << info.abort_;
    json << "stoppable" << info.stoppable_;
    json << "stopped" << info.stopped_;
    json << "canceled" << info.canceled_;
    json << "exception" << info.exception_;
    json << "cancelMsg" << info.cancelMsg_;
    json << "config" << info.config_;
    // json << "resource" << resource_;
    json << "parent" << info.parent_;
    json << "depth" << info.depth_;
    json << "state" << info.state_;
    json << "port" << info.port_;
    json << "host" << info.host_;
    json << "message" << info.message_;

    json.endObject();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
