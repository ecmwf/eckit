/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_ProducerConsumer_h
#define eckit_ProducerConsumer_h

#include "eckit/eckit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/thread/MutexCond.h"
#include "eckit/thread/Thread.h"
#include "eckit/thread/ThreadControler.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

template <class PAYLOAD>
class Producer {
public:

    virtual ~Producer() {}
    virtual bool done()            = 0;
    virtual void produce(PAYLOAD&) = 0;
};

template <class PAYLOAD>
class Consumer {
public:

    virtual ~Consumer() {}
    virtual void consume(PAYLOAD&) = 0;
};

template <class PAYLOAD>
class ProducerConsumerTask;

template <class PAYLOAD>
class ProducerConsumer {
public:

    // -- Contructors

    ProducerConsumer(long count = 2);

    // -- Destructor

    ~ProducerConsumer();

    // -- Methods

    void execute(Producer<PAYLOAD>&, Consumer<PAYLOAD>&);


    bool error();
    void error(const std::string&);

private:

    // No copy allowed

    ProducerConsumer(const ProducerConsumer&);
    ProducerConsumer& operator=(const ProducerConsumer&);

    // -- Methods

    // -- Members

    Mutex mutex_;

    long count_;


    bool error_;
    std::string why_;


    // -- Friends

    friend class ProducerConsumerTask<PAYLOAD>;
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
class ProducerConsumer;

template <class PAYLOAD>
class ProducerConsumerTask : public Thread {
    ProducerConsumer<PAYLOAD>& owner_;
    Consumer<PAYLOAD>& consumer_;
    OnePayload<PAYLOAD>* payloads_;

public:

    ProducerConsumerTask(Consumer<PAYLOAD>&, ProducerConsumer<PAYLOAD>&, OnePayload<PAYLOAD>*);
    void run() override;
};


template <class PAYLOAD>
ProducerConsumer<PAYLOAD>::ProducerConsumer(long count) : count_(count), error_(false) {}
template <class PAYLOAD>
ProducerConsumer<PAYLOAD>::~ProducerConsumer() {}

template <class PAYLOAD>
inline void ProducerConsumer<PAYLOAD>::error(const std::string& why) {
    AutoLock<Mutex> lock(mutex_);
    error_ = true;
    why_   = why;
}

template <class PAYLOAD>
inline bool ProducerConsumer<PAYLOAD>::error() {
    AutoLock<Mutex> lock(mutex_);
    return error_;
}


template <class PAYLOAD>
void ProducerConsumer<PAYLOAD>::execute(Producer<PAYLOAD>& producer, Consumer<PAYLOAD>& consumer) {

    OnePayload<PAYLOAD>* payloads = new OnePayload<PAYLOAD>[count_];

    error_ = false;

    ThreadControler thread(new ProducerConsumerTask<PAYLOAD>(consumer, *this, payloads), false);

    thread.start();

    int i = 0;

    while (!error()) {
        AutoLock<MutexCond> lock(payloads[i].cond_);

        while (payloads[i].ready_) {
            payloads[i].cond_.wait();
        }

        if (error()) {
            break;
        }

        if (producer.done()) {
            payloads[i].done_  = true;
            payloads[i].ready_ = true;
            payloads[i].cond_.signal();
            break;
        }

        try {
            producer.produce(payloads[i].payload_);
        }
        catch (std::exception& e) {
            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is handled" << std::endl;
            error(e.what());
        }

        payloads[i].ready_ = true;
        payloads[i].cond_.signal();

        i++;
        i %= count_;
    }


    thread.wait();
    delete[] payloads;

    if (error_) {
        throw SeriousBug(why_);
    }
}

template <class PAYLOAD>
ProducerConsumerTask<PAYLOAD>::ProducerConsumerTask(Consumer<PAYLOAD>& consumer, ProducerConsumer<PAYLOAD>& owner,
                                                    OnePayload<PAYLOAD>* payloads) :
    Thread(), owner_(owner), consumer_(consumer), payloads_(payloads) {}

template <class PAYLOAD>
void ProducerConsumerTask<PAYLOAD>::run() {

    int i = 0;

    while (!owner_.error()) {

        AutoLock<MutexCond> lock(payloads_[i].cond_);

        while (!payloads_[i].ready_) {
            payloads_[i].cond_.wait();
        }

        if (owner_.error()) {
            break;
        }

        if (payloads_[i].done_) {
            payloads_[i].ready_ = false;
            payloads_[i].cond_.signal();
            break;
        }

        bool error = false;

        try {
            consumer_.consume(payloads_[i].payload_);
        }
        catch (std::exception& e) {
            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is handled" << std::endl;
            owner_.error(e.what());
            error = true;
        }

        payloads_[i].ready_ = false;

        if (error) {
            ASSERT(owner_.error());
            payloads_[i].cond_.signal();
            break;
        }


        payloads_[i].cond_.signal();

        i++;
        i %= owner_.count_;
    }
}

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
