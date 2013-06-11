/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Channel.h
/// @author Tiago Quintino

#ifndef eckit_log_Channel_h
#define eckit_log_Channel_h

#include <iosfwd>

#include "eckit/log/CodeLocation.h"
#include "eckit/memory/NonCopyable.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

/// Output channel that is an std::ostream but more functional
/// @todo consider merging Channel with MultiChannel
class Channel : public std::ostream, 
                private NonCopyable {
public: // methods
    
    /// constructor takes ownership of buffer
    Channel( std::streambuf* b );    

    /// destructor deallocates buffer
    ~Channel();
    
    /// sets the code location from where next log will be
    Channel& source( const CodeLocation& );

    friend Channel& operator<<( Channel&, const CodeLocation& );
    
};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
