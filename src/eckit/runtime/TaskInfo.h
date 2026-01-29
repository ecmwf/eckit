/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Nov 96

#ifndef eckit_TaskInfo_h
#define eckit_TaskInfo_h

#include <sys/time.h>
#include <cstring>

#include "eckit/memory/Padded.h"
#include "eckit/runtime/TaskID.h"
#include "eckit/types/Types.h"

namespace eckit {

class JSON;

//----------------------------------------------------------------------------------------------------------------------

/// @warning This class is written to disk! Any change must ensure binary compatibility.
struct Info {
    bool busy_{};
    pthread_t thread_{};
    pid_t pid_{};

    time_t start_{};
    time_t last_{};
    time_t check_{};

    bool show_{};

    unsigned long late_{};

    // Logging
    static constexpr int size_{10240};
    char buffer_[size_]{};
    unsigned long pos_{};

    char name_[80]{};
    char kind_[80]{};
    char status_[256]{};
    char application_[80]{};

    // Progress

    struct Progress {
        unsigned long long min_{};
        unsigned long long max_{};
        unsigned long long val_{};
        char name_[80]{};
        double rate_{};
        double speed_{};
        ::timeval start_{};
        ::timeval last_{};
    };

    Progress progress_{};

    TaskID taskID_{};

    bool stop_{};
    bool abort_{};
    bool stoppable_{};
    bool stopped_{};
    bool canceled_{};
    bool exception_{};
    char cancelMsg_[80]{};

    int config_{};
    char resource_{};

    long parent_{};
    long depth_{};

    char state_{};

    int port_{};
    char host_[80]{};

    char message_[80]{};
};

//----------------------------------------------------------------------------------------------------------------------

/// @warning This class is written to disk! Any change must ensure binary compatibility.
class TaskInfo : public NonCopyable {
public:

    TaskInfo();
    ~TaskInfo();

    void out(char*, char*);
    bool busy(bool = false);
    const char* name() const { return info.name_; }
    const char* kind() const { return info.kind_; }
    const char* status() const { return info.status_; }
    const char* message() const { return info.message_; }
    const char* application() const { return info.application_; }
    pid_t pid() const { return info.pid_; }
    time_t last() const { return info.last_; }
    time_t start() const { return info.start_; }

    unsigned long late() const { return info.late_; }
    void late(unsigned long n) {
        touch();
        info.late_ = n;
    }

    const TaskID& taskID() { return info.taskID_; }
    void taskID(const TaskID& n) { info.taskID_ = n; }

    void stop() { info.stop_ = true; }
    bool stopped() const { return info.stopped_; }
    void abort() { info.abort_ = true; }
    void checkAbort();
    void kill(int);

    bool exception() const { return info.exception_; }
    void exception(bool on) { info.exception_ = on; }

    // ---------------------------------------------------------
    // Progress

    void start(unsigned long long, unsigned long long);
    void progress(unsigned long long);
    void done();

    unsigned long long max() const { return info.progress_.max_; }
    unsigned long long min() const { return info.progress_.min_; }
    unsigned long long val() const { return info.progress_.val_; }
    double rate() const { return info.progress_.rate_; }
    double speed() const { return info.progress_.speed_; }
    const char* progressName() const { return info.progress_.name_; }

    const ::timeval& progressStart() const { return info.progress_.start_; }
    const ::timeval& progressLast() const { return info.progress_.last_; }

    // ---------------------------------------------------------

    void kind(const std::string&);
    void name(const std::string&);
    void status(const std::string&);
    void message(const std::string&);
    void progressName(const std::string&);

    /// @todo FIXME potential race condition (reported by Clang ThreadSanitizer)
    void show(bool s) {
        touch();
        info.show_ = s;
    }
    bool show() const { return info.show_; }

    void stoppable(bool s) {
        info.stoppable_ = s;
        touch();
    }
    bool stoppable() const { return info.stoppable_; }
    bool stopTriggered() const { return info.stop_; }
    void setStopped() {
        info.stopped_ = true;
        info.stop_    = false;
    }

    void touch();

    void resource(const std::string&);

    unsigned long text(char*, unsigned long, unsigned long&) const;


    void parent(long p);
    long parent() const { return info.parent_; }
    long depth() const { return info.depth_; }

    void state(char c) {
        touch();
        info.state_ = c;
    }
    char state() const { return info.state_; }

    void port(int p) {
        touch();
        info.port_ = p;
    }
    int port() const { return info.port_; }

    void host(const std::string& h) {
        touch();
        strncpy(info.host_, h.c_str(), sizeof(info.host_));
        info.host_[sizeof(info.host_) - 1] = '\0';
    }

    std::string host() const { return info.host_; }

private:  // methods

    void print(std::ostream&) const;
    void json(JSON&) const;

    friend std::ostream& operator<<(std::ostream& s, const TaskInfo& p) {
        p.print(s);
        return s;
    }

    friend JSON& operator<<(JSON& s, const TaskInfo& p) {
        p.json(s);
        return s;
    }

    Padded<Info, 4096> info{};
};

// Used by MappedArray

inline unsigned long version(TaskInfo*) {
    return 1;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
