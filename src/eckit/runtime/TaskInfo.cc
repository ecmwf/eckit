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
#include <cstddef>

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

// -- Binary compatibility guards --
// The Info struct is memory-mapped to disk by MappedArray and SharedMemArray.
// Any change to its layout silently breaks binary compatibility with existing
// monitor files. Type-trait asserts remain in the header for consumer visibility.
//
// Memory layout (64-bit Linux, x86-64):
//
// Offset  Hex     Size   Field                Notes
// ------  ------  -----  -------------------  ----------------------------
//      0  0x0000      1  busy_                bool
//            7 bytes padding (align to 8)
//      8  0x0008      8  thread_              pthread_t
//     16  0x0010      4  pid_                 pid_t
//            4 bytes padding (align to 8)
//     24  0x0018      8  start_               time_t
//     32  0x0020      8  last_                time_t
//     40  0x0028      8  check_               time_t
//     48  0x0030      1  show_                bool
//            7 bytes padding (align to 8)
//     56  0x0038      8  late_                unsigned long
//     64  0x0040  10240  buffer_[10240]       char array (circular log)
//  10304  0x2840      8  pos_                 unsigned long
//  10312  0x2848     80  name_[80]            char array
//  10392  0x2898     80  kind_[80]            char array
//  10472  0x28E8    256  status_[256]         char array
//  10728  0x29E8     80  application_[80]     char array
//  10808  0x2A38    152  progress_            Progress sub-struct
//  10960  0x2AD0      8  taskID_              unsigned long long
//  10968  0x2AD8      1  stop_                bool
//  10969  0x2AD9      1  abort_               bool
//  10970  0x2ADA      1  stoppable_           bool
//  10971  0x2ADB      1  stopped_             bool
//  10972  0x2ADC      1  canceled_            bool
//  10973  0x2ADD      1  exception_           bool
//  10974  0x2ADE     80  cancelMsg_[80]       char array
//  11056  0x2B30      4  config_              int
//  11060  0x2B34      1  resource_            char
//            3 bytes padding (align to 8)
//  11064  0x2B38      8  parent_              long
//  11072  0x2B40      8  depth_               long
//  11080  0x2B48      1  state_               char
//            3 bytes padding (align to 4)
//  11084  0x2B4C      4  port_                int
//  11088  0x2B50     80  host_[80]            char array
//  11168  0x2BA0     80  message_[80]         char array
//  11248  0x2BF0         (end of Info)
//           1040 bytes padding_ in Padded<Info, 4096>
//  12288  0x3000         (end of TaskInfo)

static_assert(sizeof(Info) == 11248, "sizeof(Info) changed — binary compatibility broken");
static_assert(sizeof(Info::Progress) == 152, "sizeof(Info::Progress) changed — binary compatibility broken");

static_assert(offsetof(Info, busy_) == 0);
static_assert(offsetof(Info, thread_) == 8);
static_assert(offsetof(Info, pid_) == 16);
static_assert(offsetof(Info, start_) == 24);
static_assert(offsetof(Info, last_) == 32);
static_assert(offsetof(Info, check_) == 40);
static_assert(offsetof(Info, show_) == 48);
static_assert(offsetof(Info, late_) == 56);
static_assert(offsetof(Info, buffer_) == 64);
static_assert(offsetof(Info, pos_) == 10304);
static_assert(offsetof(Info, name_) == 10312);
static_assert(offsetof(Info, kind_) == 10392);
static_assert(offsetof(Info, status_) == 10472);
static_assert(offsetof(Info, application_) == 10728);
static_assert(offsetof(Info, progress_) == 10808);
static_assert(offsetof(Info, taskID_) == 10960);
static_assert(offsetof(Info, stop_) == 10968);
static_assert(offsetof(Info, abort_) == 10969);
static_assert(offsetof(Info, stoppable_) == 10970);
static_assert(offsetof(Info, stopped_) == 10971);
static_assert(offsetof(Info, canceled_) == 10972);
static_assert(offsetof(Info, exception_) == 10973);
static_assert(offsetof(Info, cancelMsg_) == 10974);
static_assert(offsetof(Info, config_) == 11056);
static_assert(offsetof(Info, resource_) == 11060);
static_assert(offsetof(Info, parent_) == 11064);
static_assert(offsetof(Info, depth_) == 11072);
static_assert(offsetof(Info, state_) == 11080);
static_assert(offsetof(Info, port_) == 11084);
static_assert(offsetof(Info, host_) == 11088);
static_assert(offsetof(Info, message_) == 11168);

// Progress sub-struct field offsets (relative to Info)
static_assert(offsetof(Info, progress_.min_) == 10808);
static_assert(offsetof(Info, progress_.max_) == 10816);
static_assert(offsetof(Info, progress_.val_) == 10824);
static_assert(offsetof(Info, progress_.name_) == 10832);
static_assert(offsetof(Info, progress_.rate_) == 10912);
static_assert(offsetof(Info, progress_.speed_) == 10920);
static_assert(offsetof(Info, progress_.start_) == 10928);
static_assert(offsetof(Info, progress_.last_) == 10944);

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
    // Use volatile write to prevent the compiler from eliminating this store as
    // a dead store. Without it, GCC -O2 DSE removes the store because all
    // subsequent destructors in the chain are trivial (no code reads busy_ after
    // this point within the process). But busy_ lives in a memory-mapped file
    // shared across processes, so the write is externally observable and must
    // not be suppressed.
    *static_cast<volatile bool*>(&info.busy_) = false;
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
