#ifndef Pipe_H
#define Pipe_H

#include "eclib/machine.h"
#include "eclib/Mutex.h"
#include "eclib/ThreadControler.h"

template<class PAYLOAD>
class PipeTask;

template<class PAYLOAD>
struct OnePayload;

template<class PAYLOAD>
class Pipe {
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

    bool   error();
    void   error(const string&);


    // No copy allowed

    Pipe(const Pipe&);
    Pipe& operator=(const Pipe&);

    // -- Methods

    // -- Members

    Mutex  mutex_;

    long   count_;

    int ri_;
    int wi_;
    int pi_;


    bool   done_;
    bool   error_;
    string why_;

    OnePayload<PAYLOAD>* payloads_;
    ThreadControler* controler_;

    // -- Friends

    friend class PipeTask<PAYLOAD>;

};

#include "eclib/Pipe.cc"

#endif
