/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <ostream>

#include "eckit/log/ChannelBuffer.h"
#include "eckit/log/LogTarget.h"
#include "eckit/exception/Exceptions.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ChannelBuffer::ChannelBuffer( std::size_t size ) :
    std::streambuf(), 
    buffer_( size + 1 ) // + 1 so we can always write the '\0'        
{
    ASSERT( size );
    char *base = &buffer_.front();
    setp(base, base + buffer_.size() - 1 ); // don't consider the space for '\0'
}

ChannelBuffer::~ChannelBuffer() 
{ 
    sync();
    for(targets_t::iterator i = targets_.begin(); i != targets_.end(); ++i) {
        delete *i;
    }
}

bool ChannelBuffer::dumpBuffer()
{
    for(targets_t::iterator i = targets_.begin(); i != targets_.end(); ++i) {
        std::ostream& os = **i;
        os.write(pbase(),pptr() - pbase());
    }
    setp(pbase(), epptr());
    return true;
}


std::streambuf::int_type ChannelBuffer::overflow(std::streambuf::int_type ch)
{
    if (ch == traits_type::eof() ) { return sync(); }
    dumpBuffer();
    sputc(ch);
    return traits_type::to_int_type(ch);
}

std::streambuf::int_type ChannelBuffer::sync()
{
    if( dumpBuffer() )
    {
        for(targets_t::iterator i = targets_.begin(); i != targets_.end(); ++i) {
            (*i)->flush();
        }
        return 0;
    }
    else return -1;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
