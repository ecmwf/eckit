/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_Pipe_h
#define eckit_Pipe_h

#include "eckit/machine.h"

#include "eckit/memory/NonCopyable.h"
#include "eckit/thread/Mutex.h"
#include "eckit/thread/ThreadControler.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

template<class PAYLOAD>
class PipeTask;

template<class PAYLOAD>
struct OnePayload;

template<class PAYLOAD>
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

    bool   error();
    void   error(const string&);

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


//-----------------------------------------------------------------------------

} // namespace eckit

#include "eckit/runtime/Pipe.cc"

#endif
