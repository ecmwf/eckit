/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <signal.h>

#include "eclib/AutoLock.h"
#include "eclib/Log.h"
#include "eclib/Monitor.h"
#include "eclib/Mutex.h"
#include "eclib/PipeHandler.h"

//-----------------------------------------------------------------------------

namespace eclib {

//-----------------------------------------------------------------------------

template<class Request>
PipeHandler<Request>::PipeHandler():
    ClassExtent<PipeHandler<Request> >(this),
    pipe_(0),
    busy_(false),
    last_(0)
{
    Monitor::show(false);
}

//=========================================================================

template<class Request>
PipeHandler<Request>::~PipeHandler()
{
    stop();
    delete pipe_;
}

//=========================================================================

template<class Request>
void PipeHandler<Request>::send(Request* r)
{

    int retry = 0;
    int max   = 10;

    static const char *here = __FUNCTION__;

    Monitor::show(true);

    for(;;)
    {

        if(!active())
            start();

        try {
            Log::status() << "Sending request" << endl;
            (*pipe_) << bool(false); // endBatch marker
            r->send(*pipe_);
            return;
        }
        catch(exception& e)
        {
            Log::error() << "** " << e.what()
                         << " Caught in " << here << endl;
            if(++retry >= max)
            {
                Log::error() << "** Exception is re-thrown" << endl;
                throw;
            }
            else
            {
                Log::error() << "** Exception is handled" << endl;
                stop();
                ::sleep(5);
            }
        }

    }

}

//=========================================================================

template<class Request>
void PipeHandler<Request>::receive(Request* r)
{
    Log::status() << "Waiting for " << Request::commandName() << endl;
    r->reply(*pipe_);
}

//=========================================================================

template<class Request>
void PipeHandler<Request>::handle(const vector<Request*>& v)
{
    static const char *here = __FUNCTION__;

    busy_ = true;

    for(typename vector<Request*>::const_iterator i = v.begin();
        i != v.end() ; ++i)
    {
        try {

            Request *r = *i;

            bool retry = false;
            int  cnt   = 0;

            do {

                retry = false;

                try {
                    send(r);
                    receive(r);
                }
                catch(exception& e)
                {
                    Log::error() << "** " << e.what() << " Caught in "
                        << here <<  endl;
                    Log::error() << "** Exception is handled" << endl;
                    retry = r->error(e,++cnt);
                }

                if(retry)
                    Log::debug() << "PipeHandler -> retry " << cnt << endl;

            } while(retry);

            Log::debug() << "PipeHandler done" << endl;
            r->done();

        }
        catch(exception& e)
        {
            Log::error() << "** " << e.what() << " Caught in " << here <<  endl;
            Log::error() << "** Exception is handled" << endl;
        }
    }

    try {
        // Send the end batch flag
        *pipe_ << bool(true);
    }
    catch(exception& e)
    {
        Log::error() << "** " << e.what() << " Caught in " << here <<  endl;
        Log::error() << "** Exception is ignored" << endl;
    }

    last_ = ::time(0);
    busy_ = false;
    Log::status() << "-" << endl;


}

template<class Request>
void PipeHandler<Request>::idle()
{
    Monitor::show(active());
}

//=========================================================================

static Mutex mutexx;

template<class Request>
void PipeHandler<Request>::start()
{

    // Here we need a mutex, because we don't want
    // to fork (ProcessControler::start) while an other
    // thread is creating a pipe. The child process
    // will then also has a file descriptor for this pipe

    AutoLock<Mutex> lock(mutexx);
    Log::debug() << "PipeHandler - Locked..." << endl;


    delete pipe_;
    pipe_ = new PipeStream();

    ProcessControler::start();

    // Here, we still are in the parent
    pipe_->parentProcess();
    Log::debug() << "PipeHandler - UnLocked..." << endl;
}

//=========================================================================

template<class Request>
void PipeHandler<Request>::stop()
{
    ProcessControler::stop();
}

//=========================================================================


template<class Request>
void PipeHandler<Request>::run()
{
    // Here, we should be in the child
    pipe_->childProcess();


    char in[20];  snprintf(in,20,"%d", pipe_->in());
    char out[20]; snprintf(out,20,"%d",pipe_->out());
    char par[20]; snprintf(par,20,"%ld",Monitor::self());

    PathName cmd = string("~/bin/") + Request::commandName();

    Log::debug() << "execlp(" << cmd.localPath() << ','
                 << cmd.baseName().localPath() << ','
                 << "-in," << in << ','
                 << "-out," << out << ','
                 << "-parent," << par << ")" << endl;

#if 0
    if(getenv("PIPE_DEBUG"))
    {
        ::execlp(getenv("PIPE_DEBUG"),getenv("PIPE_DEBUG"),
            cmd.c_str(), "-in",in, "-out",out, "-parent",par, 0);

        cerr << "Exec failed " << getenv("PIPE_DEBUG") << Log::syserr  << endl;
        ::kill(getpid(),SIGTERM);
    }
    else
#endif

    char command[1024];
    char basename[1024];

    ASSERT(sizeof(command)-1 > string(cmd).length());

    snprintf(command,1024,"%s", cmd.localPath());
    snprintf(basename,1024,"%s", cmd.baseName().localPath());

    ::execlp(command, basename,
        "-in",in,
        "-out",out,
        "-parent",par,
        (void*)0);

    cerr << "Exec failed " << cmd << Log::syserr << endl;

    // exit is not enough: some destructor of ostore objects
    // are waiting for some locks
    ::kill(getpid(),SIGTERM);
}

template<class Request>
void PipeHandler<Request>::ready(bool& r)
{
    r = r || (active() && !busy());
}

template<class Request>
void PipeHandler<Request>::age(time_t& a)
{
    if(active() && !busy())
        if(last_ > a )
            a = last_;
}

//=========================================================================
// Check if an other thread is avialable and as already forked.
// In this case don't pick the request otherwise, choose it

template<class Request>
bool PipeHandler<Request>::canPick()
{
    time_t a = 0;
    ClassExtent<PipeHandler<Request> >::callAll(&PipeHandler<Request>::age,a);

    // If someone is more ready me
    if( a > last_)
    {
        Log::debug() << "canPick " << a << " > "
        << last_ << " size="
        << ClassExtent<PipeHandler<Request> >::size() << endl;
        return false;
    }

    bool ok = false;
    ready(ok);

    // If ready, do it

    if(ok) return true;

    ClassExtent<PipeHandler<Request> >::callAll(&PipeHandler<Request>::ready,ok);

    // If no one else ready, do it

    if(ok)
    {
        Log::debug() << "canPick size=" <<
        ClassExtent<PipeHandler<Request> >::size() << endl;
    }

    return !ok;

}

template<class Request>
void PipeHandler<Request>::pick(list<Request*>& queue,
    vector<Request*>& result)
{

    if(canPick())
        DefaultHandler<Request>::pick(queue,result);
}

template<class Request>
void PipeHandler<Request>::endBatch(Stream&)
{
}

//-----------------------------------------------------------------------------

} // namespace eclib

