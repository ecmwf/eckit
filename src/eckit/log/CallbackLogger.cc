/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cassert>

#include "eckit/log/CallbackLogger.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/thread/Mutex.h"
#include "eckit/thread/Once.h"
#include "eclib/TimeStamp.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

static Once<Mutex> mutex;

class CallbackBuffer : public std::streambuf,
                       private NonCopyable {
public:
        
    CallbackBuffer( std::size_t size = 1024 ) : 
        call_(0),
        level_(0),
        ctxt_(0),
        internal_buf_( size + 1 ) // + 1 so we can always write the '\0'
    {
        assert( size );
        char *base = &internal_buf_.front();
        setp(base, base + internal_buf_.size() - 1 ); // don't consider the space for '\0'
    }

    void register_callback( CallbackLogger::callback c, int level, void* ctxt )
    {
        call_  = c;
        level_ = level;
        ctxt_  = ctxt;
    }
    
    protected:
        bool dumpBuffer();

    private:
        int_type overflow(int_type ch);
        int sync();

    private:
        CallbackLogger::callback call_;
        int level_;
        void* ctxt_;
        std::vector<char> internal_buf_;
};

std::streambuf::int_type CallbackBuffer::overflow( std::streambuf::int_type ch )
{
    AutoLock<Mutex> lock(mutex);
    if (ch == traits_type::eof() ) {
        return sync();
    }

    dumpBuffer();
    sputc(ch);

    return ch;    
}

std::streambuf::int_type CallbackBuffer::sync()
{
    AutoLock<Mutex> lock(mutex);
	return dumpBuffer() ? 0 : -1;
}

bool CallbackBuffer::dumpBuffer()
{    
    if( call_ && pbase() != pptr() ) // forward the output to the callback function
    {
        // insert '\0' at pptr() -- e can do it because we reserved an extra character
        
        *pptr() = '\0';                       
        pbump(1);
        
        // callback -- context can be null
        
        (*call_)( ctxt_, level_, pbase() );
    }

    // rollback -- even if no forward was made
    
    std::ptrdiff_t n = pptr() - pbase();
    pbump(-n);
        
    return true;
}

//-----------------------------------------------------------------------------

CallbackLogger::CallbackLogger()
{
    buffer_ = new CallbackBuffer();
    out_ = new std::ostream( buffer_ );
}

CallbackLogger::~CallbackLogger()
{
    delete out_;
    delete buffer_;
}

void CallbackLogger::beginLine()
{
    // nothing here, we just forward the input to the callback without modifications
}

void CallbackLogger::endLine()
{
    // nothing here, we just forward the input to the callback without modifications
}

std::ostream& CallbackLogger::out()
{
    return *out_;
}

void CallbackLogger::register_callback( callback c, int level, void* ctxt )
{
    AutoLock<Mutex> lock(mutex);

    assert( c );

    // context 'ctxt' may be null
    
    buffer_->register_callback( c, level, ctxt );
}

//-----------------------------------------------------------------------------

} // namespace eckit
