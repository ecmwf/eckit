/*
 * © Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/thread/ThreadControler.h"
#include "eckit/thread/Thread.h"
#include "eckit/thread/Mutex.h"
#include "eckit/thread/MutexCond.h"
#include "eckit/thread/AutoLock.h"


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

template<class PAYLOAD>
struct OnePayload {
    MutexCond cond_;
    bool      ready_;
    bool       done_;
    PAYLOAD   payload_;
    OnePayload(): ready_(false), done_(false), payload_() {}
};

template<class PAYLOAD>
class ProducerConsumer;

template<class PAYLOAD>
class ProducerConsumerTask : public Thread {
    ProducerConsumer<PAYLOAD>&         owner_;
    Consumer<PAYLOAD>&        consumer_;
    OnePayload<PAYLOAD>*  payloads_;
public:
    ProducerConsumerTask(Consumer<PAYLOAD>&,ProducerConsumer<PAYLOAD>&,OnePayload<PAYLOAD>*);
    virtual void run();
};


template<class PAYLOAD>
ProducerConsumer<PAYLOAD>::ProducerConsumer(long count):
    count_(count),
    error_(false)
{

}
template<class PAYLOAD>
ProducerConsumer<PAYLOAD>::~ProducerConsumer()
{
}

template<class PAYLOAD>
inline void ProducerConsumer<PAYLOAD>::error(const string& why)
{
    AutoLock<Mutex> lock(mutex_);
    error_ = true;
    why_   = why;
}

template<class PAYLOAD>
inline bool ProducerConsumer<PAYLOAD>::error()
{
    AutoLock<Mutex> lock(mutex_);
    return error_;
}


template<class PAYLOAD>
void ProducerConsumer<PAYLOAD>::execute(Producer<PAYLOAD>& producer,Consumer<PAYLOAD>& consumer)
{

    static const char *here = __FUNCTION__;

    OnePayload<PAYLOAD>* payloads = new OnePayload<PAYLOAD>[count_];



    error_   = false;

    ThreadControler thread(new ProducerConsumerTask<PAYLOAD>(consumer,*this,payloads),false);

    thread.start();

    int i = 0;

    while(!error())
    {
        AutoLock<MutexCond> lock(payloads[i].cond_);

        while(payloads[i].ready_)
            payloads[i].cond_.wait();

        if(error())
            break;

        if(producer.done())
        {
            payloads[i].done_ = true;
            payloads[i].ready_ = true;
            payloads[i].cond_.signal();
            break;
        }

        try {
            producer.produce(payloads[i].payload_);
        }
        catch(exception& e)
        {
            Log::error() << "** " << e.what() << " Caught in " <<
                            here <<  endl;
            Log::error() << "** Exception is handled" << endl;
            error(e.what());
        }

        payloads[i].ready_ = true;
        payloads[i].cond_.signal();

        i++;
        i %= count_;

    }



    thread.wait();
    delete[] payloads;

    if(error_) {
        throw SeriousBug(why_);
    }


}

template<class PAYLOAD>
ProducerConsumerTask<PAYLOAD>::ProducerConsumerTask(Consumer<PAYLOAD>& consumer,
                                                    ProducerConsumer<PAYLOAD>& owner,
                                                    OnePayload<PAYLOAD>* payloads):
    Thread(false),
    consumer_(consumer),
    owner_(owner),
    payloads_(payloads)
{
}

template<class PAYLOAD>
void ProducerConsumerTask<PAYLOAD>::run()
{

    static const char *here = __FUNCTION__;

    int i = 0;

    while(!owner_.error())
    {

        AutoLock<MutexCond> lock(payloads_[i].cond_);

        while(!payloads_[i].ready_)
            payloads_[i].cond_.wait();

        if(owner_.error())
            break;

        if(payloads_[i].done_) {
            payloads_[i].ready_ = false;
            payloads_[i].cond_.signal();
            break;
        }

        bool error = false;

        try {
            consumer_.consume(payloads_[i].payload_);
        }
        catch(exception& e)
        {
            Log::error() << "** " << e.what() << " Caught in " <<
                            here <<  endl;
            Log::error() << "** Exception is handled" << endl;
            owner_.error(e.what());
            error = true;
        }

        payloads_[i].ready_ = false;

        if(error)
        {
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

} // namespace eckit

