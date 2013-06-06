/*
 * (C) Copyright 1996-2013 ECMWF.
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

#include <vector>
#include <streambuf>
#include <cassert>

#include "eckit/log/OStreamHandle.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class FormatBuffer: public std::streambuf {
public:

    FormatBuffer( std::size_t size = 1024 );

    virtual ~FormatBuffer();

    void target(std::ostream* os) { os_.reset(os); }
    void target(std::ostream& os) { os_.reset(os); }
    std::ostream* target() const  { return os_.get(); }
    
protected: // members
    
    ostream_handle os_;
    bool start_;
    std::vector<char> buffer_;

protected: // methods
    
    virtual void beginLine() {}
    virtual void endLine() {}
    virtual void process( const char* begin, const char* end );

    bool dumpBuffer();

private:

    virtual int_type overflow(int_type ch);

    virtual int_type sync();
};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
