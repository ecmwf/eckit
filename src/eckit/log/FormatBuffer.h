/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file FormatBuffer.h
/// @author Tiago Quintino

#ifndef eckit_log_FormatBuffer_h
#define eckit_log_FormatBuffer_h

#include "eckit/log/ChannelBuffer.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class FormatBuffer: public ChannelBuffer {
public:

    FormatBuffer( std::ostream* os = 0, std::size_t size = 1024 );
    FormatBuffer( std::ostream& os, std::size_t size = 1024 );

    virtual ~FormatBuffer();
    
protected: // members
    
    bool start_;

protected: // methods
    
    virtual void beginLine() {}
    virtual void endLine() {}
    
    virtual void process( const char* begin, const char* end );

    virtual bool dumpBuffer();
};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
