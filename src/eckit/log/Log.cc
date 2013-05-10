/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/compat/Inited.h"
#include "eckit/log/Log.h"
#include "eckit/log/LogBuffer.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/thread/ThreadSingleton.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

// This assume that iostream are thread safe during a single
// call to the << inserter

//=============================================================================


StatusStream::StatusStream():
    ostream(new StatusBuffer)
{
}

StatusStream::~StatusStream()
{
        delete (StatusBuffer*)rdbuf();
}

StatusBuffer::StatusBuffer():
        streambuf()
{
        setp(buffer_, buffer_ + sizeof(buffer_));
        setg(0, 0, 0);
}

StatusBuffer::~StatusBuffer()
{
        //dumpBuffer();
}

int	StatusBuffer::overflow(int c)
{
        if (c == EOF) {
                sync();
                return 0;
        }

        dumpBuffer();
        sputc(c);
        return 0;
}

int	StatusBuffer::sync()
{
        dumpBuffer();
        return 0;
}

void StatusBuffer::dumpBuffer()
{
        ::replace(pbase(),pptr(),'\n','\0');
        Monitor::status(pbase());
        setp(pbase(), epptr());
}
//=============================================================================

MessageStream::MessageStream():
    ostream(new MessageBuffer)
{
}

MessageStream::~MessageStream()
{
        delete (MessageBuffer*)rdbuf();
}

MessageBuffer::MessageBuffer():
        streambuf()
{
        setp(buffer_, buffer_ + sizeof(buffer_));
        setg(0, 0, 0);
}

MessageBuffer::~MessageBuffer()
{
        dumpBuffer();
}

int	MessageBuffer::overflow(int c)
{
        if (c == EOF) {
                sync();
                return 0;
        }

        dumpBuffer();
        sputc(c);
        return 0;
}

int	MessageBuffer::sync()
{
        dumpBuffer();
        return 0;
}

void MessageBuffer::dumpBuffer()
{
        ::replace(pbase(),pptr(),'\n','\0');
        Monitor::message(pbase());
        setp(pbase(), epptr());
}

//=============================================================================

MonitorStream::MonitorStream():
    ostream(new MonitorBuffer)
{
}

MonitorStream::~MonitorStream()
{
        delete (MonitorBuffer*)rdbuf();
}

void MonitorStream::flags(char type,long mode)
{
        ((MonitorBuffer*)rdbuf())->flags(type,mode);
}

MonitorBuffer::MonitorBuffer():
    streambuf(),
    start_(true)
{
        setp(buffer_, buffer_ + sizeof(buffer_));
        setg(0, 0, 0);
}

MonitorBuffer::~MonitorBuffer()
{
        dumpBuffer();
}

int	MonitorBuffer::overflow(int c)
{
        if (c == EOF) {
                sync();
                return 0;
        }

        dumpBuffer();
        sputc(c);
        return 0;
}

int	MonitorBuffer::sync()
{
        dumpBuffer();
        return 0;
}

// constance#marsb#19970925:001609#ANR0400I Session 1200 started for node MARS...
void MonitorBuffer::dumpBuffer()
{
        setp(pbase(), epptr());
        cerr << flush;
}

//=============================================================================

void Log::lock()
{
    //mutex.lock();
}

void Log::unlock()
{
    //mutex.unlock();
}

//=============================================================================
ostream& Log::monitor(char type,long mode)
{
        static ThreadSingleton<MonitorStream> x;
        MonitorStream& y = x.instance();
        y.flags(type,mode);
        return y;
}


ostream& Log::status()
{
        static ThreadSingleton<StatusStream> x;
        return x.instance();
}

ostream& Log::message()
{
        static ThreadSingleton<MessageStream> x;
        return x.instance();
}

ostream& Log::info()
{
    return Context::instance().infoStream();
}

ostream& Log::info(const CodeLocation& where)
{
    return Context::instance().infoStream().source(where);
}

ostream& Log::error()
{
    return Context::instance().errorStream();
}

ostream& Log::error(const CodeLocation& where)
{
    return Context::instance().infoStream().source(where);
}

ostream& Log::panic()
{
        try
        {
                return Log::error();
        }
        catch(exception&)
        {
                return cerr;
        }
}

ostream& Log::panic(const CodeLocation& where)
{
        try 
        {
                return Log::error(where);
        }
        catch(exception&)
        {
                cerr << "[" << where << "]";
                return cerr;
        }
}

ostream& Log::warning()
{
    return Context::instance().warnStream();
}

ostream& Log::warning(const CodeLocation& where)
{
    return Context::instance().warnStream().source(where);
}

ostream& Log::debug(int level)
{
        static ofstream devnull("/dev/null");
        if(level >= Context::instance().debug())
                return devnull;
        else
            return Context::instance().debugStream();
}

ostream& Log::debug(const CodeLocation& where, int level)
{
        static ofstream devnull("/dev/null");
        if(level >= Context::instance().debug())
                return devnull;
        else
            return Context::instance().debugStream().source(where);
}

#ifndef _GNU_SOURCE

static void handle_strerror_r(ostream& s, int e, char es[], int hs )
{
    if( hs == 0 )
        s << " (" << es << ") " ;
    else
        s << " (errno = " << e << ") ";
}

#else // GNU implementation is not XSI compliant and returns char* instead of int

static void handle_strerror_r(ostream& s, int e, char[], char* p )
{
    if( p != 0 )
        s << " (" << p << ") " ;
    else
        s << " (errno = " << e << ") ";
}

#endif

ostream& Log::syserr(ostream& s)
{
        int e = errno;
        char estr [256];
        handle_strerror_r(s, e, estr, strerror_r( e, estr, sizeof(estr) ) );
        return s;
}

static int xindex = ios::xalloc();

int format(ostream& s)
{
        return s.iword(xindex);
}

ostream& setformat(ostream& s,int n)
{
        s.iword(xindex) = n;
        return s;
}


SaveStatus::SaveStatus():
        status_(Monitor::status())
{
}

SaveStatus::~SaveStatus()
{
        Monitor::status(status_);
}

//=============================================================================

UserOutStream::UserOutStream():
    ostream(new UserOutBuffer(this))
{
}

UserOutStream::~UserOutStream()
{
        delete (UserOutBuffer*)rdbuf();
}

UserOutBuffer::UserOutBuffer(UserOutStream* owner):
        streambuf(),
        owner_(owner)
{
        setp(buffer_, buffer_ + sizeof(buffer_));
        setg(0, 0, 0);
}

UserOutBuffer::~UserOutBuffer()
{
        dumpBuffer();
}

int	UserOutBuffer::overflow(int c)
{
        if (c == EOF) {
                sync();
                return 0;
        }

        dumpBuffer();
        sputc(c);
        return 0;
}

int	UserOutBuffer::sync()
{
        dumpBuffer();
        return 0;
}

void UserOutBuffer::dumpBuffer()
{
        ::replace(pbase(),epptr(),'\n','\0');
        owner_->out(pbase());
        setp(pbase(), epptr());
}

//=============================================================================

typedef Inited<UserStream*> UserStreamP;

static UserStreamP& getUserStream()
{
        static ThreadSingleton<UserStreamP> x;
        return x.instance();
}

void UserInfoStream::out(const string& msg)
{
        UserStreamP& h = getUserStream();
        UserStream*  x = h;
        if(x) x->infoMsg(msg);
}

#if 0
static void _debug(const char* a,const char *b)
{
        static int check = 0;
        if(check) return;
        check = 1;

        try {

                UserStreamP& h = getUserStream();
                UserStream*  x = h;
                if(x) x->infoMsg(a);
        }
        catch(exception&)
        {
                Log::setUserStream(0);
                check = 0;
        }

        check = 0;
}
#endif

void UserWarningStream::out(const string& msg)
{
        UserStreamP& h = getUserStream();
        UserStream*  x = h;
        if(x) x->warningMsg(msg);
}

void UserErrorStream::out(const string& msg)
{
        UserStreamP& h = getUserStream();
        UserStream*  x = h;
        if(x) x->errorMsg(msg);
}

void Log::notifyClient(const string& msg)
{
        UserStreamP& h = getUserStream();
        UserStream*  x = h;
        if(x) x->notifyClient(msg);
}

void Log::setUserStream(UserStream* s)
{
        UserStreamP& h = getUserStream();
        h = s;
}

ostream& Log::userInfo()
{
        static ThreadSingleton<UserInfoStream> x;
        return x.instance();
}

ostream& Log::userError()
{
        static ThreadSingleton<UserErrorStream> x;
        return x.instance();
}

ostream& Log::userWarning()
{
        static ThreadSingleton<UserWarningStream> x;
        return x.instance();
}


// Explicit instantiations.
template class ThreadSingleton<Exception*>;
template class ThreadSingleton<InfoStream>;
template class ThreadSingleton<WarningStream>;
template class ThreadSingleton<MessageStream>;
template class ThreadSingleton<ErrorStream>;
template class ThreadSingleton<MonitorStream>;
template class ThreadSingleton<UserInfoStream>;
template class ThreadSingleton<DebugStream>;
template class ThreadSingleton<UserStreamP>;
template class ThreadSingleton<UserWarningStream>;
template class ThreadSingleton<StatusStream>;
template class ThreadSingleton<UserErrorStream>;

//-----------------------------------------------------------------------------

} // namespace eckit
