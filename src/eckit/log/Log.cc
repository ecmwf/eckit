/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/Log.h"
#include "eckit/log/LogBuffer.h"
#include "eckit/runtime/Monitor.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

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
        Monitor::instance().status(pbase());
        setp(pbase(), epptr());
}

//-----------------------------------------------------------------------------

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
        Monitor::instance().message(pbase());
        setp(pbase(), epptr());
}

//-----------------------------------------------------------------------------

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

void MonitorBuffer::dumpBuffer()
{
        setp(pbase(), epptr());
        cerr << flush;
}

//-----------------------------------------------------------------------------

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
    return Context::instance().infoChannel();
}

ostream& Log::info(const CodeLocation& where)
{
    return Context::instance().infoChannel().source(where);
}

ostream& Log::error()
{
    return Context::instance().errorChannel();
}

ostream& Log::error(const CodeLocation& where)
{
    return Context::instance().infoChannel().source(where);
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
    return Context::instance().warnChannel();
}

ostream& Log::warning(const CodeLocation& where)
{
    return Context::instance().warnChannel().source(where);
}

ostream& Log::debug(int level)
{
        static ofstream devnull("/dev/null");
        if(level >= Context::instance().debug())
                return devnull;
        else
            return Context::instance().debugChannel();
}

ostream& Log::debug(const CodeLocation& where, int level)
{
        static ofstream devnull("/dev/null");
        if(level >= Context::instance().debug())
                return devnull;
        else
            return Context::instance().debugChannel().source(where);
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

ostream& Log::syserr(ostream& s)
{
        int e = errno;
        char estr [256];
        handle_strerror_r(s, e, estr, strerror_r( e, estr, sizeof(estr) ) );
        return s;
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

/// @note some explicit template instantiations
///
template class ThreadSingleton<Exception*>;
//template class ThreadSingleton<InfoStream>;
//template class ThreadSingleton<WarnStream>;
template class ThreadSingleton<MessageStream>;
//template class ThreadSingleton<ErrorStream>;
template class ThreadSingleton<MonitorStream>;
//template class ThreadSingleton<DebugStream>;

//-----------------------------------------------------------------------------

} // namespace eckit
