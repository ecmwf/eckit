/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Dispatcher.h
// Baudouin Raoult - ECMWF Jul 96

#ifndef eckit_Dispatcher_h
#define eckit_Dispatcher_h

#include <unistd.h>
#include <algorithm>
#include <list>

#include "eckit/config/Configurable.h"
#include "eckit/config/Resource.h"
#include "eckit/log/JSON.h"
#include "eckit/log/Log.h"
#include "eckit/log/Plural.h"
#include "eckit/runtime/Dispatcher.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/runtime/Monitorable.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/thread/MutexCond.h"
#include "eckit/thread/Thread.h"
#include "eckit/thread/ThreadControler.h"

//-----------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------

class JSON;

template <class Request>
struct DefaultHandler {
    // Pick first request from queue and put it in result queue
    void pick(std::list<Request*>&, std::vector<Request*>&);
    // No-op
    void idle() {}
    // Print string representation of request to stream
    static void print(std::ostream&, const Request&);
    // Emit JSON representation of request
    static void json(JSON&, const Request&);
};

template <class Request>
struct DequeuePicker {
    /// Destructor
    virtual ~DequeuePicker() {}
    /// Pick a request from the queue
    virtual void pick(std::list<Request*>&) = 0;
};

//=======================================
// A 'T' must have a default constructor a operator()(R*)
//

template <class Traits>
class Dispatcher : public Configurable {
public:

    typedef typename Traits::Request Request;
    typedef typename Traits::Handler Handler;

    // -- Contructors

    Dispatcher(const std::string& name = Traits::name(), int numberOfThreads = 1);

    // -- Destructor

    ~Dispatcher();

    // -- Operators

    // Push a new Request in the queue
    // (Dispatcher takes ownership of Request)
    void push(Request*);

    // Push a vector of Requests in the queue
    void push(const std::vector<Request*>&);

    // Process a request from the queue
    bool next(Handler&, std::vector<Request*>&, Mutex&);

    // Remove a request from the queue without processing
    void dequeue(DequeuePicker<Request>&);

    // Sleep for 0 seconds and yield
    void sleep();

    // Sleep for a given number of seconds and yield
    void sleep(int);

    // Signal all sleeping threads
    void awake();

    // Get the queue size
    int size() const;

    // Increment the running state by given delta
    void running(long);

    // Get the running state
    long running() const;

    // Serialise requests in queue as JSON array
    void json(JSON&) const;

    // From Configurable

    virtual std::string name() const { return name_; }

protected:

    // -- Members

    // Request queue
    std::list<Request*> queue_;
    // Maximum number of threads (if set to 0, a new thread is
    // created for each request pushed into the queue)
    Resource<long> numberOfThreads_;
    // Number of currently running threads
    long count_;
    // Counter for thread ids
    long next_;
    // Mutex protecting queue_
    mutable MutexCond ready_;
    // Mutex for thread sleep/wake handling
    MutexCond wait_;
    // Dispatcher name
    std::string name_;
    // Do we dynamically grow the number of threads?
    bool grow_;
    // Number of active threads (i.e. processing work)
    long running_;

    mutable Mutex lock_;

private:

    // No copy allowed

    Dispatcher(const Dispatcher<Traits>&);
    Dispatcher<Traits>& operator=(const Dispatcher<Traits>&);

    // -- Methods

    void print(std::ostream&) const;
    void changeThreadCount(int delta);
    void _push(Request*);

    // From Configurable

    virtual std::string kind() const { return "Dispatcher"; }
    virtual void reconfigure();

    friend std::ostream& operator<<(std::ostream& s, const Dispatcher<Traits>& p) {
        p.print(s);
        return s;
    }
};

//-----------------------------------------------------------------

// Worker thread (managed by ThreadController)

template <class Traits>
class DispatchTask : public Thread, public Monitorable {
public:

    typedef typename Traits::Handler Handler;
    typedef typename Traits::Request Request;

    // -- Contructors

    DispatchTask(Dispatcher<Traits>& owner, int id) :
        Thread(false),  // Don't delete object when stopped
        owner_(owner),
        id_(id) {}

private:

    // -- Members

    // Dispatcher owning this thread
    Dispatcher<Traits>& owner_;
    // Worker thread id
    int id_;
    // Requests to be handled by this thread
    std::vector<Request*> pick_;
    // Mutex protecting the queue
    Mutex mutex_;

    // -- Overridden methods

    // From Thread

    void run() override;

    // From Monitorable

    void status(std::ostream&) const override;
    void json(JSON&) const override;


    // No copy

    typedef class DispatchTask<Traits> DT;
    DispatchTask(const DT&);
    void operator=(const DT&);
};

//-----------------------------------------------------------------

// Dispatcher thread (managed by ThreadController)

template <class Traits>
class DispatchInfo : public Thread {
    // Dispatcher owning this thread
    Dispatcher<Traits>& owner_;
    void run() override;

public:

    DispatchInfo(Dispatcher<Traits>& owner) : owner_(owner) {}
};

//=================================================================

template <class Traits>
void DispatchTask<Traits>::status(std::ostream& s) const {
    AutoLock<Mutex> lock(((DispatchTask<Traits>*)this)->mutex_);
    s << "Picks for " << owner_.name() << " thread " << id_ << std::endl;
    for (size_t i = 0; i < pick_.size(); i++) {
        if (pick_[i]) {
            Handler::print(s, *pick_[i]);
        }
    }
}

template <class Traits>
void DispatchTask<Traits>::json(JSON& s) const {
    AutoLock<Mutex> lock(((DispatchTask<Traits>*)this)->mutex_);
    int n = 0;
    for (size_t i = 0; i < pick_.size(); i++) {
        if (pick_[i]) {
            n++;
        }
    }

    if (n) {
        s.startObject();
        s << "name" << owner_.name();
        s << "id" << id_;
        s << "picks";
        s.startList();
        for (size_t i = 0; i < pick_.size(); i++) {
            if (pick_[i]) {
                Handler::json(s, *pick_[i]);
            }
        }
        s.endList();
        s.endObject();
    }
}

template <class Traits>
void DispatchTask<Traits>::run() {
    Log::info() << "Start of " << owner_.name() << " thread " << id_ << std::endl;

    Monitor::instance().name(owner_.name());

    Handler handler;

    while (!stopped()) {

        bool stop = owner_.next(handler, pick_, mutex_);

        if (stop)  // The thread must stop
        {
            Log::info() << "Stopping thread " << id_ << std::endl;
            break;
        }

        owner_.running(1);
        try {
            if (pick_.size()) {
                handler.handle(pick_);
                owner_.awake();  // Request is finished
            }
            else {
                handler.idle();
                owner_.sleep();  // Wait for something else to come...
            }
        }
        catch (std::exception& e) {
            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is ignored" << std::endl;
            owner_.awake();
        }
        owner_.running(-1);

        AutoLock<Mutex> lock(mutex_);

        for (typename std::vector<Request*>::iterator i = pick_.begin(); i != pick_.end(); ++i) {
            delete (*i);
            *i = 0;
        }
    }

    owner_.awake();
    Log::info() << "End of thread " << id_ << std::endl;
}

//=================================================================

template <class Traits>
void DispatchInfo<Traits>::run() {
    Monitor::instance().name(owner_.name());
    for (;;) {
        int n = owner_.size();
        Monitor::instance().show(n > 0);
        Log::status() << Plural(n, "request") << " queued" << std::endl;
        ::sleep(1);
        /* owner_.sleep(3); */
        /* owner_.awake(); // Awake others */
    }
}

//=================================================================

template <class Traits>
Dispatcher<Traits>::Dispatcher(const std::string& name, int numberOfThreads) :
    // Maximum number of threads defined on the command line or
    // in config file or default to the argument value
    numberOfThreads_(this, "-numberOfThreads;numberOfThreads", numberOfThreads),
    count_(0),
    next_(0),
    name_(name),
    // Dynamically grow number of threads if set to 0
    grow_(numberOfThreads_ == 0),
    running_(0) {
    // For some reason xlC require that
    typedef class DispatchInfo<Traits> DI;

    ThreadControler c(new DI(*this));
    c.start();

    // Spin up appropriate number of threads
    changeThreadCount(numberOfThreads_);
}


template <class Traits>
Dispatcher<Traits>::~Dispatcher() {}

template <class Traits>
long Dispatcher<Traits>::running() const {
    if (grow_) {
        AutoLock<Mutex> lock(lock_);
        return running_;
    }
    return 0;
}

template <class Traits>
void Dispatcher<Traits>::running(long delta) {
    if (grow_) {
        AutoLock<Mutex> lock(lock_);
        running_ += delta;
        ASSERT(running_ >= 0);
    }
}

template <class Traits>
void Dispatcher<Traits>::push(Request* r) {
    if (!r) {
        return;
    }

    if (grow_) {
        int cnt = running();
        if (cnt == count_) {
            changeThreadCount(1);
        }
    }

    _push(r);
}

template <class Traits>
void Dispatcher<Traits>::_push(Request* r) {

    {
        AutoLock<MutexCond> lock(ready_);
        queue_.push_back(r);  // enqueue Request
        ready_.signal();
    }
    awake();
}

template <class Traits>
void Dispatcher<Traits>::sleep() {
    // Log::debug() << "Sleeping..." << std::endl;
    AutoLock<MutexCond> lock(wait_);
    wait_.wait();
    // Log::debug() << "Awaken..." << std::endl;
}

template <class Traits>
void Dispatcher<Traits>::sleep(int n) {
    AutoLock<MutexCond> lock(wait_);
    wait_.wait(n);
}

template <class Traits>
void Dispatcher<Traits>::awake() {
    Log::debug() << "Awake ..." << std::endl;
    AutoLock<MutexCond> lock(wait_);
    wait_.broadcast();
}

template <class Traits>
int Dispatcher<Traits>::size() const {
    AutoLock<MutexCond> lock(ready_);
    return queue_.size();
}

template <class Traits>
void Dispatcher<Traits>::print(std::ostream& s) const {
    AutoLock<MutexCond> lock(ready_);
    for (typename std::list<Request*>::const_iterator i = queue_.begin(); i != queue_.end(); ++i) {
        if (*i) {
            Handler::print(s, *(*i));
        }
    }
}

template <class Traits>
void Dispatcher<Traits>::json(JSON& s) const {
    AutoLock<MutexCond> lock(ready_);
    s.startList();
    for (typename std::list<Request*>::const_iterator i = queue_.begin(); i != queue_.end(); ++i) {
        if (*i) {
            Handler::json(s, *(*i));
        }
    }
    s.endList();
}

template <class Traits>
bool Dispatcher<Traits>::next(Handler& handler, std::vector<Request*>& result, Mutex& mutex) {
    Log::status() << "-" << std::endl;

    AutoLock<MutexCond> lock(ready_);

    bool stop = false;

    while (queue_.empty()) {
        ready_.wait();
    }

    AutoLock<Mutex> lock2(mutex);  // Lock std::vector

    result.clear();

    // Check for null requests, which serve as thread stop signal

    typename std::list<Request*>::iterator i = std::find(queue_.begin(), queue_.end(), (Request*)0);

    // No null requests found, pick next task from the queue
    if (i == queue_.end()) {
        handler.pick(queue_, result);
        // A null request was found
    }
    else {
        // Clear the null request and stop the thread
        queue_.erase(i);
        stop = true;
    }

    Log::debug() << "Got " << result.size() << " requests from the queue" << std::endl;

    Log::debug() << "Left " << queue_.size() << " requests in the queue" << std::endl;

    ready_.signal();  // Let other threads get some more

    if (result.size()) {
        Log::status() << "Processing request" << std::endl;
    }

    return stop;
}

template <class Traits>
void Dispatcher<Traits>::dequeue(DequeuePicker<Request>& p) {
    AutoLock<MutexCond> lock(ready_);
    p.pick(queue_);
    ready_.signal();
}

template <class Traits>
void Dispatcher<Traits>::changeThreadCount(int delta) {
    // We do not take a lock, to avoid dead-locks
    typedef class DispatchTask<Traits> DT;

    // Increase number of threads by starting some more
    if (delta > 0) {
        for (int i = 0; i < delta; i++) {
            ThreadControler c(new DT(*this, next_++));
            c.start();
        }
    }

    // Decrease number of threads by pushing null requests in
    // the queue, which will cause threads to stop
    if (delta < 0) {
        for (int i = 0; i < -delta; i++) {
            _push(0);
        }
    }

    if (delta) {
        AutoLock<Mutex> lock(lock_);
        count_ += delta;
    }
}

template <class Traits>
void Dispatcher<Traits>::reconfigure() {
    Log::info() << "Reconfiguring maximum thread number to: " << numberOfThreads_ << std::endl;
    changeThreadCount(numberOfThreads_ - count_);
    awake();
}

//=================================================================

template <class Request>
void DefaultHandler<Request>::pick(std::list<Request*>& queue, std::vector<Request*>& result) {
    Request* r = queue.front();
    queue.pop_front();
    result.push_back(r);
}

template <class Request>
void DefaultHandler<Request>::print(std::ostream& s, const Request& r) {
    s << r << std::endl;
}

template <class Request>
void DefaultHandler<Request>::json(JSON& s, const Request& r) {
    r.json(s);
}

//-----------------------------------------------------------------

}  // namespace eckit

#endif
