/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_Pipe_h
#define eckit_Pipe_h

#include "eckit/eckit.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/log/CodeLocation.h"
#include "eckit/memory/NonCopyable.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/thread/MutexCond.h"
#include "eckit/thread/Thread.h"
#include "eckit/thread/ThreadControler.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

template <class PAYLOAD>
class PipeTask;

template <class PAYLOAD>
struct OnePayload;

template <class PAYLOAD>
class Pipe : private NonCopyable {
public:

    typedef void (*Proc)(Pipe<PAYLOAD>&);

    // -- Contructors

    Pipe(long count = 2);

    // -- Destructor

    ~Pipe();

    // -- Methods

    void spawn(Proc proc);

    bool eof();
    void close();

    PAYLOAD& message();
    PAYLOAD& receive();
    void send();

private:

    bool error();
    void error(const std::string&);

    // -- Methods

    // -- Members

    Mutex mutex_;

    long count_;

    int ri_;
    int wi_;
    int pi_;


    bool done_;
    bool error_;
    std::string why_;

    OnePayload<PAYLOAD>* payloads_;
    ThreadControler* controler_;

    // -- Friends

    friend class PipeTask<PAYLOAD>;
};

//-----------------------------------------------------------------------------

template <class PAYLOAD>
struct OnePayload {
    MutexCond cond_;
    bool ready_;
    bool done_;
    PAYLOAD payload_;
    OnePayload() : ready_(false), done_(false), payload_() {}
};

template <class PAYLOAD>
class Pipe;

template <class PAYLOAD>
class PipeTask : public Thread {

    typedef typename Pipe<PAYLOAD>::Proc Proc;

    Pipe<PAYLOAD>& owner_;
    Proc& consumer_;
    OnePayload<PAYLOAD>* payloads_;

public:

    PipeTask(Proc&, Pipe<PAYLOAD>&, OnePayload<PAYLOAD>*);
    void run() override;
};


template <class PAYLOAD>
Pipe<PAYLOAD>::Pipe(long count) :
    count_(count), ri_(0), wi_(0), pi_(-1), done_(false), error_(false), payloads_(0), controler_(0) {}
template <class PAYLOAD>
Pipe<PAYLOAD>::~Pipe() {
    close();
}

template <class PAYLOAD>
void Pipe<PAYLOAD>::close() {
    if (controler_) {
        {
            AutoLock<Mutex> lock(mutex_);
            done_ = true;
        }
        {
            AutoLock<MutexCond> lock(payloads_[wi_].cond_);
            payloads_[wi_].ready_ = true;
            payloads_[wi_].cond_.signal();
        }
        controler_->wait();
    }
    delete[] payloads_;
    controler_ = 0;
    payloads_  = 0;
}


template <class PAYLOAD>
inline void Pipe<PAYLOAD>::error(const std::string& why) {
    AutoLock<Mutex> lock(mutex_);
    error_ = true;
    why_   = why;
}

template <class PAYLOAD>
inline bool Pipe<PAYLOAD>::error() {
    AutoLock<Mutex> lock(mutex_);
    return error_;
}


template <class PAYLOAD>
void Pipe<PAYLOAD>::spawn(Proc proc) {

    payloads_ = new OnePayload<PAYLOAD>[count_];

    error_ = false;

    controler_ = new ThreadControler(new PipeTask<PAYLOAD>(proc, *this, payloads_), false);
    controler_->start();
}

template <class PAYLOAD>
PipeTask<PAYLOAD>::PipeTask(Proc& consumer, Pipe<PAYLOAD>& owner, OnePayload<PAYLOAD>* payloads) :
    Thread(false), owner_(owner), consumer_(consumer), payloads_(payloads) {}

template <class PAYLOAD>
void PipeTask<PAYLOAD>::run() {
    try {
        consumer_(owner_);
    }
    catch (std::exception& e) {
        Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
        Log::error() << "** Exception is handled" << std::endl;
        owner_.error(e.what());
    }
}

template <class PAYLOAD>
bool Pipe<PAYLOAD>::eof() {
    AutoLock<Mutex> lock(mutex_);
    return done_;
}

template <class PAYLOAD>
PAYLOAD& Pipe<PAYLOAD>::message() {
    AutoLock<MutexCond> lock(payloads_[wi_].cond_);

    while (payloads_[wi_].ready_) {
        payloads_[wi_].cond_.wait();
    }

    if (error()) {
        AutoLock<Mutex> lck(mutex_);
        done_  = true;
        error_ = false;
        throw SeriousBug(why_);
    }

    return payloads_[wi_].payload_;
}

template <class PAYLOAD>
void Pipe<PAYLOAD>::send() {
    AutoLock<MutexCond> lock(payloads_[wi_].cond_);

    payloads_[wi_].ready_ = true;
    payloads_[wi_].cond_.signal();

    wi_++;
    wi_ %= count_;
}

template <class PAYLOAD>
PAYLOAD& Pipe<PAYLOAD>::receive() {

    {
        AutoLock<Mutex> lck(mutex_);
        if (pi_ >= 0) {
            AutoLock<MutexCond> lock(payloads_[pi_].cond_);
            payloads_[pi_].ready_ = false;
            payloads_[pi_].cond_.signal();
        }
    }

    AutoLock<MutexCond> lock(payloads_[ri_].cond_);

    while (!payloads_[ri_].ready_) {
        payloads_[ri_].cond_.wait();
    }

    if (error()) {
        AutoLock<Mutex> lck(mutex_);
        throw SeriousBug(why_);
    }

    pi_ = ri_;

    PAYLOAD& p = payloads_[ri_].payload_;
    ri_++;
    ri_ %= count_;

    return p;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
