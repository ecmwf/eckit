/*
 * (C) Copyright 1996-2013 ECMWF.
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
#include "eckit/thread/Mutex.h"


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

template<class PAYLOAD>
class Producer {
public:
    virtual bool done() = 0;
    virtual void produce(PAYLOAD&) = 0;
};

template<class PAYLOAD>
class Consumer {
public:
    virtual void consume(PAYLOAD&) = 0;
};

template<class PAYLOAD>
class ProducerConsumerTask;

template<class PAYLOAD>
class ProducerConsumer {
public:

    // -- Contructors

    ProducerConsumer(long count = 2);

    // -- Destructor

    ~ProducerConsumer();

    // -- Methods

    void execute(Producer<PAYLOAD>&, Consumer<PAYLOAD>&);


    bool   error();
    void   error(const string&);

private:

    // No copy allowed

    ProducerConsumer(const ProducerConsumer&);
    ProducerConsumer& operator=(const ProducerConsumer&);

    // -- Methods

    // -- Members

    Mutex  mutex_;

    long   count_;


    bool   error_;
    string why_;


    // -- Friends

    friend class ProducerConsumerTask<PAYLOAD>;

};


//-----------------------------------------------------------------------------

} // namespace eckit

#include "eckit/runtime/ProducerConsumer.cc"

#endif
