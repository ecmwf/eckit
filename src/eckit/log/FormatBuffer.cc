/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <ostream>

#include "eckit/log/FormatBuffer.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

FormatBuffer::FormatBuffer(std::size_t size) : 
    std::streambuf(),
    os_(),
    start_(true),
    buffer_( size + 1 ) // + 1 so we can always write the '\0'
{
    assert( size );
    char *base = &buffer_.front();
    setp(base, base + buffer_.size() - 1 ); // don't consider the space for '\0'
}

FormatBuffer::~FormatBuffer()
{
}

void FormatBuffer::process(const char *begin, const char *end)
{
    target()->write(begin,end-begin);
}

bool FormatBuffer::dumpBuffer()
{
    /* AutoLock<Mutex> lock(mutex); */
    if( target() )
    {
        const char *p = pbase();
        const char *begin = p;
        while( p != pptr() )
        {
            if(start_)
            {
                this->beginLine();
                start_ = false;
            }
    
            if(*p == '\n')
            {
                this->process(begin,p);
                this->endLine();
                *os_ << *p;
                begin = ++p;
                start_ = true;
                continue;
            }            
            p++;
        }
        this->process(begin,p);
    }    
    setp(pbase(), epptr());
    return true;
}

std::streambuf::int_type FormatBuffer::overflow(std::streambuf::int_type ch)
{
    /* AutoLock<Mutex> lock(mutex); */
    if (ch == traits_type::eof() ) { return sync(); }
    this->dumpBuffer();
    sputc(ch);
    return ch;
}

std::streambuf::int_type FormatBuffer::sync()
{
    if( dumpBuffer() )
    {
        if( target() ) target()->flush();
        return 0;
    }
    else return -1;
}

//-----------------------------------------------------------------------------

} // namespace eckit
