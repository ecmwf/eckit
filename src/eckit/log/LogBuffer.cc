/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/log/Log.h"
#include "eclib/Logger.h"
#include "eclib/LogBuffer.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Once.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

// This is a mutex that serialize all calls to LogStream
// This is overkilled, but it protects cout and cerr.
// Unfortunatly, it will also serialize other usage of this class (e.g. sockets...)

static Once<Mutex> mutex;

LogStream::LogStream(Logger* logger):
    ostream( new LogBuffer( logger ) )
{
    setformat(*this,Log::compactFormat);
}

LogStream::~LogStream()
{
    delete (LogBuffer*)rdbuf();
}

LogBuffer::LogBuffer( Logger* logger ) :
    streambuf(),
    logger_(logger),
    start_(true),
    line_(0),
    len_(0)
{
    AutoLock<Mutex> lock(mutex);
    setp(buffer_, buffer_ + sizeof(buffer_));
    setg(0, 0, 0);
    //	unbuffered(1);
}

void LogBuffer::source(const CodeLocation &where)
{
    AutoLock<Mutex> lock(mutex);
    logger_->location(where);
}

LogBuffer::~LogBuffer()
{
    AutoLock<Mutex> lock(mutex);
    dumpBuffer();
    logger_->out() << flush;
}

int	LogBuffer::overflow(int c)
{
    AutoLock<Mutex> lock(mutex);
    if (c == EOF) {
        sync();
        return 0;
    }

    dumpBuffer();
    sputc(c);

    return 0;
}

int	LogBuffer::sync()
{
    AutoLock<Mutex> lock(mutex);
    dumpBuffer();
    start_ = true;
    logger_->out() << flush;
    return 0;
}

void LogBuffer::dumpBuffer(void)
{
    AutoLock<Mutex> lock(mutex);
    
    Monitor::out(pbase(),pptr());
    
    const char *p = pbase();
    
    while(p != pptr())
    {
        if(start_)
        {
            logger_->beginLine();
            start_ = false;
        }

        if(*p == '\n')
        {
            logger_->endLine();
            start_ = true;
        }

        logger_->out() << *p;
        p++;
    }

    setp(pbase(), epptr());

    logger_->out() << flush;
}

//-----------------------------------------------------------------------------

} // namespace eckit

