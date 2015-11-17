/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file ChannelBuffer.h
/// @author Tiago Quintino

#ifndef eckit_log_ChannelBuffer_h
#define eckit_log_ChannelBuffer_h

#include <vector>
#include <streambuf>

#include "eckit/memory/NonCopyable.h"
#include "eckit/log/CodeLocation.h"
#include "eckit/log/OStreamHandle.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

/// Generic base for buffers
class ChannelBuffer : public std::streambuf,
                      private NonCopyable {

public: // methods
    
    /// constructor, taking ownership of stream
    ChannelBuffer( std::ostream* os = 0, std::size_t size = 1024 );
    
    /// constructor, not taking ownership of stream
    ChannelBuffer( std::ostream& os, std::size_t size = 1024 );

    /// destructor, deallocates stream if has ownership
    ~ChannelBuffer();
    
    /// access the target stream
    /// may return null pointer so always check before dereferencing
    std::ostream* target() const  { return os_.get(); }    

    /// access the target stream
    /// @returns if there is a target stream set for this buffer
    bool has_target() const  { return os_.get(); }
    
    /// sets the target stream, passing ownership
    void target(std::ostream* os) { os_.reset(os); }
    /// sets the target stream, not passing ownership
    void target(std::ostream& os) { os_.reset(os); }    
    
    /// sets the current location of the code outputing
    void location( const CodeLocation& );
    
protected: // methods

    /// override this to change buffer behavior
    /// @returns true if no error occured
    virtual bool dumpBuffer();
    
    /// typically you don't need to override this
    /// @see dumpBuffer
    virtual int_type overflow(int_type ch);

    /// typically you don't need to override this
    /// @see dumpBuffer
    virtual int_type sync();
    
protected: // members

    ostream_handle os_;
    bool hasLoc_;
    CodeLocation where_;
    std::vector<char> buffer_;
};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
