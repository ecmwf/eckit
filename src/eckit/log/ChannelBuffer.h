/*
 * (C) Copyright 1996-2016 ECMWF.
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

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class LogTarget;

/// Generic base for buffers
class ChannelBuffer : public std::streambuf,
                      private NonCopyable {

public: // methods
    
    /// constructor, taking ownership of stream
    ChannelBuffer( std::size_t size = 1024 );
    
    /// destructor, deallocates stream if has ownership
    virtual ~ChannelBuffer();

    size_t targets() const { return targets_.size(); }
            
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

    typedef std::vector<LogTarget*> targets_t;

    std::vector<LogTarget*> targets_;

    std::vector<char> buffer_;

};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
