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

FormatBuffer::FormatBuffer(std::ostream *os, std::size_t size) : 
    ChannelBuffer(os,size),
    start_(true)
{
}

FormatBuffer::FormatBuffer(std::ostream &os, std::size_t size) :
    ChannelBuffer(os,size),
    start_(true)
{
}

FormatBuffer::~FormatBuffer()
{
}

void FormatBuffer::process(const char *begin, const char *end)
{
    if( has_target() )
        target()->write(begin,end - begin);
}

bool FormatBuffer::dumpBuffer()
{
    /* AutoLock<Mutex> lock(mutex); */
    if( has_target() )
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

//-----------------------------------------------------------------------------

} // namespace eckit
