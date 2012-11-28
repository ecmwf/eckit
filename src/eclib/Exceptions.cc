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

#include "eclib/Exceptions.h"
#include "eclib/StrStream.h"
#include "eclib/ThreadSingleton.h"

#include "eclib/BackTrace.h"

static Exception*& first()
{
    static ThreadSingleton<Exception*> p;
    return p.instance();
}

void xldb_throw(const char *c) // To set a break point in xldb
{
    if(getenv("PAUSE_EXCEPTIONS"))
    {
        cout << "debug me " << getpid() << endl;
        pause();
    }

    if(getenv("ABORT_EXCEPTIONS"))
        Panic(c);
}

Exception::Exception():
    next_(first())
{
    first() = this;

#if defined( PSTACK_COMMAND )
    std::ostringstream cmd;
    
#if defined( CPPFILT_COMMAND )
    cmd << PSTACK_COMMAND << getpid() << " | " << CPPFILT_COMMAND;
#else
    cmd << PSTACK_COMMAND << getpid();
#endif
    
    FILE* f = popen(cmd.str().c_str(), "r");
    if(f)
    {
        char buffer[1024];
        int n;
        while ((n = fread(buffer, 1, sizeof buffer, f)) > 0)
            std::cerr.write(buffer, n);
        pclose(f);
    }
#endif
#ifndef AIX
    std::cerr << "BACKTRACE: " << BackTrace::dump() << std::endl;
#endif

    xldb_throw("?");
}

Exception::~Exception() throw()
{
    first() = next_;
}

void Exception::exceptionStack(ostream& out)
{
    out << "Exception stack: " << endl;
    Exception* e =  first();
    while(e)
    {
        out << e->what() << endl;
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

    std::cerr << "BACKTRACE: " << BackTrace::dump() << std::endl;

    first() = this;
    xldb_throw(w.c_str());
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

FailedSystemCall::FailedSystemCall(const char* msg,int line,
        const char* file, const char* proc,int err)
{
    StrStream s;

    errno = err;
    s << "Failed system call: " << msg << " in " << proc 
        << ", line " << line << " of " << file << Log::syserr << StrStream::ends;

    reason(string(s));
    Log::monitor(Log::Unix,errno) << what() << endl;
}

FailedSystemCall::FailedSystemCall(const string& ctx, const char* msg,int line,
        const char* file, const char* proc,int err)
{
    StrStream s;

    errno = err;
    s << "Failed system call: " << msg << " in " << proc 
        << ", line " << line << " of " << file << Log::syserr << " [" << ctx << "]" << StrStream::ends;

    reason(string(s));
    Log::monitor(Log::Unix,errno) << what() << endl;
}

AssertionFailed::AssertionFailed(const string& w): 
    Exception(string("Assertion failed: ") + w)
{   
    Log::monitor(Log::Other,1) << what() << endl;
} 

AssertionFailed::AssertionFailed(const char* msg, const CodeLocation& loc)
{
    StrStream s;

    s << "Assertion failed: " << msg << " in " << loc.func()
        << ", line " << loc.line() << " of " << loc.file() << StrStream::ends;

    reason(string(s));
    Log::monitor(Log::Other,2) << what() << endl;
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
    Log::monitor(Log::Other,2) << what() << endl;
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
    std::cout << "cannot open file [" << file << "]" << std::endl;
    StrStream s;
    s << "Cannot open " << file << " " << Log::syserr;
    if(retry) s << " (retry ok)";
    s << StrStream::ends;
    reason(string(s));
    Log::monitor(Log::Unix,errno) << what() << endl;
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

void Panic(const char *msg)
{

    msg = msg ? msg : "(null message)";

    cout << "PANIC: " << msg << endl;
    cerr << "PANIC: " << msg << endl;

    Log::monitor(Log::Other,9999) << msg << endl;

    Log::panic() << "PANIC IS CALLED!!!" << endl;
    Log::panic() << msg << endl;

    if(getenv("SLEEP_ON_PANIC"))
    {
        Log::panic() << "Use dbx -a " << getpid() << " or xldb -a " << getpid() << endl;
        ::kill(::getpid(),SIGSTOP);
    }
    else ::kill(::getpid(),SIGABRT);
    ::pause();
}

void Panic(const char* msg,int line,const char* file, const char* proc)
{
    StrStream s;
    s << msg << " in " << proc << ", line " << line << " of " << file << StrStream::ends;
    string t(s);
    Panic(t.c_str());
}

OutOfMemory::OutOfMemory():
    Exception("out of memory")
{
}
