/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <unistd.h>
#include <signal.h>

#include "eckit/exception/Exceptions.h"
#include "eckit/compat/StrStream.h"
#include "eckit/thread/ThreadSingleton.h"

#include "eckit/os/BackTrace.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

static Exception*& first()
{
    static ThreadSingleton<Exception*> p;
    return p.instance();
}

Exception::Exception():  next_(first())
{
    first() = this;

    callStack_ = BackTrace::dump();
}

Exception::~Exception() throw()
{
    first() = next_;
}

void Exception::print(ostream & out) const
{
    out << what_;
}

void Exception::exceptionStack(ostream& out, bool callStack)
{
    out << "Exception stack: " << endl;
    Exception* e =  first();
    while(e)
    {
        out << e->what() << endl;

        if(callStack)
            out << e->callStack() << endl << endl;

        e = e->next_;
    }
    out << "End stack" << endl;
}

Exception::Exception(const string& w, const CodeLocation& location):
    what_(w),
    next_(first()),
    location_(location)
{
    Log::error() << "Exception: " << w << " @ " << location_ << std::endl;

#if 0
    if(next_) {
        Log::error() << "Exception: stack containts " << next_->what() << endl;
    }
    else
    {
        Log::error() << "Exception: stack is empty" << endl;
    }
#endif

    callStack_ = BackTrace::dump();

    first() = this;
    Log::status() << "** " << w << " @ " << location_ << std::endl;
}

void Exception::reason(const string& w)
{
    Log::error() << "Exception: " << w << endl;
    what_ = w;
}

bool Exception::throwing() 
{
    return first() != 0;
}

TooManyRetries::TooManyRetries(const int retries) 
{   
    StrStream s;
    s << "Too many retries: " << retries << StrStream::ends;
    reason(string(s)); 
    Log::monitor(Log::Unix,errno) << what() << endl;
}

TimeOut::TimeOut(const string& msg, const unsigned long timeout)
{   
    StrStream s;
    s  << "Timeout expired: " << timeout << " (" << msg << ")" << StrStream::ends ;
    reason(string(s));
}


FailedSystemCall::FailedSystemCall(const string& w)
{   
    StrStream s;
    s << "Failed system call: " << w << " " << Log::syserr << StrStream::ends;
    reason(string(s));
    Log::monitor(Log::Unix,errno) << what() << endl;
}

FailedSystemCall::FailedSystemCall(const string& msg, const CodeLocation& loc)
{
    StrStream s;
    s << "Failed system call: " << msg << " " << " in " << loc << " " << Log::syserr << StrStream::ends;
    reason(string(s));
    Log::monitor(Log::Unix,errno) << what() << endl;
}

FailedSystemCall::FailedSystemCall(const char* msg,const CodeLocation& loc,int err)
{
    StrStream s;

    errno = err;
    s << "Failed system call: " << msg << " in " << loc << " " << Log::syserr << StrStream::ends;

    reason(string(s));
    Log::monitor(Log::Unix,errno) << what() << endl;
}

FailedSystemCall::FailedSystemCall(const string& ctx, const char* msg, const CodeLocation& loc,int err)
{
    StrStream s;

    errno = err;
    s << "Failed system call: " << msg << " in " << loc << " " << Log::syserr << " [" << ctx << "]" << StrStream::ends;

    reason(string(s));
    Log::monitor(Log::Unix,errno) << what() << endl;
}

AssertionFailed::AssertionFailed(const string& w): 
    Exception(string("Assertion failed: ") + w)
{   
    Log::monitor(Log::App,1) << what() << endl;
} 

AssertionFailed::AssertionFailed(const char* msg, const CodeLocation& loc)
{
    StrStream s;

    s << "Assertion failed: " << msg << " in " << loc.func()
        << ", line " << loc.line() << " of " << loc.file() << StrStream::ends;

    reason(string(s));
    Log::monitor(Log::App,2) << what() << endl;
}

BadParameter::BadParameter(const string& w):
    Exception(string("Bad parameter: ") + w)
{   
}

NotImplemented::NotImplemented( const CodeLocation& loc )
{   
    StrStream s;

    s << "Not implemented: " << loc.func()
        << ", line " << loc.line() << " of " << loc.file() << StrStream::ends;

    reason(string(s));
    Log::monitor(Log::App,2) << what() << endl;
    string t = string(s);
    //	Panic(t.c_str());

}

UserError::UserError(const string& r):
    Exception(string("UserError: ") + r)
{   
}

UserError::UserError(const string& r,const string& x):
    Exception(string("UserError: ") + r + " : " + x)
{   
}

Stop::Stop(const string& r):
    Exception(string("Stop: ") + r)
{   
}

Abort::Abort(const string& r):
    Exception(string("Abort: ") + r)
{   
}

Retry::Retry(const string& r):
    Exception(string("Retry: ") + r)
{
}

Cancel::Cancel(const string& r):
    Exception(string("Cancel: ") + r)
{   
}

OutOfRange::OutOfRange(unsigned long long index, unsigned long long max)
{   
    StrStream s;
    s << "Out of range accessing element " << index 
        << ", but maximum is " << max - 1 << StrStream::ends;
    reason(string(s));
}

FileError::FileError(const string& msg)
{   
    StrStream s;
    s << msg <<  Log::syserr;
    s << StrStream::ends;
    reason(string(s));
    Log::monitor(Log::Unix,errno) << what() << endl;
}

CantOpenFile::CantOpenFile(const string& file, bool retry):
    retry_(retry)
{   
    /* std::cout << "cannot open file [" << file << "]" << std::endl; */
    StrStream s;
    s << "Cannot open " << file << " " << Log::syserr;
    if(retry) s << " (retry ok)";
    s << StrStream::ends;
    reason(string(s));
    Log::monitor(Log::Unix,errno) << what() << endl;
}

MethodNotYetImplemented::MethodNotYetImplemented(const string &msg):
    Exception(string("Method not yet implemented: " + msg))
{
}

WriteError::WriteError(const string& file): 
    FileError(string("Write error on ") + file)
{   
}

ReadError::ReadError(const string& file): 
    FileError(string("Read error on ") + file)
{   
}

ShortFile::ShortFile(const string& file): 
    ReadError(string("Short file while reading ") + file)
{   
}

RemoteException::RemoteException(const string& msg, const string& from):
    Exception(msg + "(RemoteException from " + from + ")")
{
}

void handle_panic(const char *msg)
{
    msg = msg ? msg : "(null message)";

    cout << "PANIC: " << msg << endl;
    cerr << "PANIC: " << msg << endl;

    Log::monitor(Log::App,9999) << msg << endl;

    Log::panic() << "PANIC IS CALLED!!!" << endl;
    Log::panic() << msg << endl;
    
    Log::panic() << "----------------------------------------\n"
                 << "BACKTRACE\n" 
                 << "----------------------------------------\n"
                 << BackTrace::dump() << std::endl
                 << "----------------------------------------\n"
                 << std::endl;

    if(getenv("SLEEP_ON_PANIC"))
    {
        Log::panic() << "Use dbx -a " << getpid() << " or xldb -a " << getpid() << endl;
        ::kill(::getpid(),SIGSTOP);
    }
    else 
        ::kill(::getpid(),SIGABRT);
    
    ::pause();
}

void handle_panic(const char* msg, const CodeLocation& location )
{
    StrStream s;
    s << msg << " in " << location << StrStream::ends;
    string t(s);
    handle_panic(t.c_str());
}

OutOfMemory::OutOfMemory():
    Exception("out of memory")
{
}

//-----------------------------------------------------------------------------

} // namespace eckit
