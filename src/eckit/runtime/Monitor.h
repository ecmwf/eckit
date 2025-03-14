/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @date   Jun 1996
/// @author Baudouin Raoult
/// @author Tiago Quintino

#ifndef eckit_Monitor_h
#define eckit_Monitor_h

#include "eckit/eckit.h"

#include "eckit/filesystem/PathName.h"
#include "eckit/memory/NonCopyable.h"
#include "eckit/runtime/TaskInfo.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/ThreadSingleton.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class Monitor : private NonCopyable {

public:  // types

    class TaskArray : private eckit::NonCopyable {

    public:

        typedef TaskInfo* iterator;
        typedef const TaskInfo* const_iterator;

        virtual ~TaskArray();

        virtual void sync()   = 0;
        virtual void lock()   = 0;
        virtual void unlock() = 0;

        virtual iterator begin()             = 0;
        virtual iterator end()               = 0;
        virtual const_iterator begin() const = 0;
        virtual const_iterator end() const   = 0;
        virtual const_iterator cbegin() const { return begin(); }
        virtual const_iterator cend() const { return end(); }

        virtual unsigned long size()                  = 0;
        virtual TaskInfo& operator[](unsigned long n) = 0;
    };

public:  // methods

    static Monitor& instance();

    static bool active();
    static void active(bool a);

    void reset();  //< use in forked processes only
    void startup();
    void shutdown();

    void out(char*, char*);  // called from Log

    void name(const std::string&);
    void kind(const std::string&);

    void progress(const std::string&, unsigned long long, unsigned long long);
    void progress(unsigned long long);
    void progress();

    char state(char);

    void message(const std::string&);
    std::string message();

    void status(const std::string&);
    std::string status();
    std::string statusTree();
    void stoppable(bool);
    bool stopTriggered();
    void setStopped();
    bool stopped();

    long self();
    void parent(long);

    void start(const std::string&);
    int kill(const std::string&, int = 15);

    void port(int);
    int port();

    void host(const std::string&);
    std::string host();

    TaskID taskID();
    void taskID(const TaskID&);

    void show(bool);

    void cancel(const std::string&);
    std::string cancelMessage();
    bool canceled();

    void abort();
    void checkAbort();

    TaskInfo& task(unsigned long slot);
    TaskArray& tasks();


private:  // members

    unsigned long slot_;
    bool ready_;
    bool check_;

private:  // methods
          /// Contructors

    Monitor();

    /// Destructor

    ~Monitor();

    unsigned long hash();

    TaskInfo& task();

    void init();

    friend class ThreadSingleton<Monitor>;
    friend struct NewAlloc0<Monitor>;
};

//----------------------------------------------------------------------------------------------------------------------

// Wrap the setting of Monitor::instance().state

class AutoState {
    char old_;

public:

    AutoState(char c) : old_(Monitor::instance().state(c)) {}

    ~AutoState() { Monitor::instance().state(old_); }

    void set(char c) { Monitor::instance().state(c); }
};

//----------------------------------------------------------------------------------------------------------------------

template <class T>
class AutoLockTag {
    AutoState state_;
    AutoLock<T> lock_;

public:

    AutoLockTag(T& t) : state_(t.tag()), lock_(t) { state_.set(t.tag() - 'a' + 'A'); }
    AutoLockTag(T* t) : state_(t->tag()), lock_(t) { state_.set(t->tag() - 'a' + 'A'); }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
