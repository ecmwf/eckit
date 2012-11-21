#ifndef ProducerConsumer_H
#define ProducerConsumer_H

#include "eclib/machine.h"
#include "eclib/Mutex.h"

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

#include "eclib/ProducerConsumer.cc"

#endif
