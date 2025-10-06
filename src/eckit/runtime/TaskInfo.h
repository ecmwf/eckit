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

struct Info {
protected:

    bool busy_;
    pthread_t thread_;
    pid_t pid_;

    time_t start_;
    time_t last_;
    time_t check_;

    bool show_;

    unsigned long late_;

    // Logging
    enum {
        size_ = 10240
    };
    char buffer_[size_];
    unsigned long pos_;

    char name_[80];
    char kind_[80];
    char status_[256];
    char application_[80];

    // Progress

    struct Progress {
        unsigned long long min_;
        unsigned long long max_;
        unsigned long long val_;
        char name_[80];
        double rate_;
        double speed_;
        ::timeval start_;
        ::timeval last_;
    };

    Progress progress_;

    TaskID taskID_;

    bool stop_;
    bool abort_;
    bool stoppable_;
    bool stopped_;
    bool canceled_;
    bool exception_;
    char cancelMsg_[80];

    int config_;
    char resource_;

    long parent_;
    long depth_;

    char state_;

    int port_;
    char host_[80];

    char message_[80];
};

//----------------------------------------------------------------------------------------------------------------------

class TaskInfo : public Padded<Info, 4096>, public NonCopyable {
public:

    TaskInfo();
    ~TaskInfo();

    void out(char*, char*);
    bool busy(bool = false);
    const char* name() const { return name_; }
    const char* kind() const { return kind_; }
    const char* status() const { return status_; }
    const char* message() const { return message_; }
    const char* application() const { return application_; }
    pid_t pid() const { return pid_; }
    time_t last() const { return last_; }
    time_t start() const { return start_; }

    unsigned long late() const { return late_; }
    void late(unsigned long n) {
        touch();
        late_ = n;
    }

    const TaskID& taskID() { return taskID_; }
    void taskID(const TaskID& n) { taskID_ = n; }

    void stop() { stop_ = true; }
    bool stopped() const { return stopped_; }
    void abort() { abort_ = true; }
    void checkAbort();
    void kill(int);

    bool exception() const { return exception_; }
    void exception(bool on) { exception_ = on; }

    // ---------------------------------------------------------
    // Progress

    void start(unsigned long long, unsigned long long);
    void progress(unsigned long long);
    void done();

    unsigned long long max() const { return progress_.max_; }
    unsigned long long min() const { return progress_.min_; }
    unsigned long long val() const { return progress_.val_; }
    double rate() const { return progress_.rate_; }
    double speed() const { return progress_.speed_; }
    const char* progressName() const { return progress_.name_; }

    const ::timeval& progressStart() const { return progress_.start_; }
    const ::timeval& progressLast() const { return progress_.last_; }

    // ---------------------------------------------------------

    void kind(const std::string&);
    void name(const std::string&);
    void status(const std::string&);
    void message(const std::string&);
    void progressName(const std::string&);

    /// @todo FIXME potential race condition (reported by Clang ThreadSanitizer)
    void show(bool s) {
        touch();
        show_ = s;
    }
    bool show() const { return show_; }

    void stoppable(bool s) {
        stoppable_ = s;
        touch();
    }
    bool stoppable() const { return stoppable_; }
    bool stopTriggered() const { return stop_; }
    void setStopped() {
        stopped_ = true;
        stop_    = false;
    }

    void touch();

    void resource(const std::string&);

    unsigned long text(char*, unsigned long, unsigned long&) const;


    void parent(long p);
    long parent() const { return parent_; }
    long depth() const { return depth_; }

    void state(char c) {
        touch();
        state_ = c;
    }
    char state() const { return state_; }

    void port(int p) {
        touch();
        port_ = p;
    }
    int port() const { return port_; }

    void host(const std::string& h) {
        touch();
        strncpy(host_, h.c_str(), sizeof(host_));
        host_[sizeof(host_) - 1] = '\0';
    }

    std::string host() const { return host_; }

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
};

// Used by MappedArray

inline unsigned long version(TaskInfo*) {
    return 1;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
