/*
 * (C) Copyright 1996-2015 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <ostream>

#include "eckit/log/ChannelBuffer.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

ChannelBuffer::ChannelBuffer( std::ostream* os, std::size_t size ) : 
    std::streambuf(), 
    os_(os),
    hasLoc_(false),
    buffer_( size + 1 ) // + 1 so we can always write the '\0'        
{
    assert( size );
    char *base = &buffer_.front();
    setp(base, base + buffer_.size() - 1 ); // don't consider the space for '\0'
}

ChannelBuffer::ChannelBuffer( std::ostream& os, std::size_t size ) : 
    std::streambuf(), 
    os_(os),
    hasLoc_(false),
    buffer_( size + 1 ) // + 1 so we can always write the '\0'        
{
    assert( size );
    char *base = &buffer_.front();
    setp(base, base + buffer_.size() - 1 ); // don't consider the space for '\0'
}

ChannelBuffer::~ChannelBuffer() 
{ 
    sync(); 
}

void ChannelBuffer::location(const CodeLocation& where)
{
    hasLoc_ = true;
    where_ = where;    
}

bool ChannelBuffer::dumpBuffer()
{
    if( has_target() )
        target()->write(pbase(),pptr() - pbase());
    setp(pbase(), epptr());
    return true;
}

std::streambuf::int_type ChannelBuffer::overflow(std::streambuf::int_type ch)
{
    /* AutoLock<Mutex> lock(local_mutex); */
    if (ch == traits_type::eof() ) { return sync(); }
    dumpBuffer();
    sputc(ch);
    return traits_type::to_int_type(ch);
}

std::streambuf::int_type ChannelBuffer::sync()
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
