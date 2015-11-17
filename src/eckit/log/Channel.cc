/*
 * (C) Copyright 1996-2015 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/exception/Exceptions.h"

#include "eckit/log/Channel.h"
#include "eckit/log/ChannelBuffer.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

Channel::Channel(std::streambuf *b) : std::ostream( b )
{
    ASSERT(b);
}

Channel::~Channel() { delete rdbuf(); }

Channel& Channel::source(const CodeLocation& where)
{
    ChannelBuffer* buf = dynamic_cast<ChannelBuffer*>(rdbuf());
    if (buf) 
        buf->location(where);
    return *this;
}

Channel& operator<<( Channel& ch, const CodeLocation & loc)
{
    return ch.source(loc);
}

//-----------------------------------------------------------------------------

} // namespace eckit
