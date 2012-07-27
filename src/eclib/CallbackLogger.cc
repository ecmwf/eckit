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

#include "eclib/CallbackLogger.h"
#include "eclib/Monitor.h"
#include "eclib/TimeStamp.h"

//-----------------------------------------------------------------------------

class CallbackBuffer : public std::streambuf,
                       public NonCopyable {
public:
        
    CallbackBuffer( std::size_t size = 1024 ) : 
        call_(0),
        level_(0),
        ctxt_(0),
        internal_buf_( size + 2 ) // smallest possible size of buffer is 1 plus another for the '\0'
    {
        char *base = &internal_buf_.front();
        setp(base, base + internal_buf_.size() - 1); // -1 to make overflow() easier
    }

    void register_callback( CallbackLogger::callback* c, int level, void* ctxt )
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
        CallbackLogger::callback* call_;
        int level_;
        void* ctxt_;
        std::vector<char> internal_buf_;
};

CallbackBuffer::int_type CallbackBuffer::overflow(int_type ch)
{
    if( ch != traits_type::eof() )
    {
        assert(std::less_equal<char *>()(pptr(), epptr()));
        *pptr() = ch; // we can always write because we reserved an extra character
        pbump(1);
        if( dumpBuffer() ) return ch; // success returns anything but traits_type::eof()
    }

    return traits_type::eof();
}

int CallbackBuffer::sync()
{
	return dumpBuffer() ? 0 : -1;
}

bool CallbackBuffer::dumpBuffer()
{    
    // forward the output to the callback function
    
    if( call_ )
    {
        // insert '\0' at pptr()
        // we can always write '\0' because we reserved an extra character
        
        *pptr() = '\0';                       
        pbump(1);
        
        // callback -- context can be null
        
        (*call_)( ctxt_, level_, pbase() );
    }

    // rollback -- even if no forward was made
    
    std::ptrdiff_t n = pptr() - pbase();
    pbump(-n);
        
    return 0;
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

void CallbackLogger::register_callback( callback* c, int level, void* ctxt )
{
    assert( c );
    assert( ctxt );
    
    buffer_->register_callback( c, level, ctxt );
}
