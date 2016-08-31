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
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   August 2016

#ifndef eckit_log_ChannelBuffer_h
#define eckit_log_ChannelBuffer_h

#include <vector>
#include <streambuf>

#include "eckit/memory/NonCopyable.h"
#include "eckit/log/Channel.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class LogTarget;

/// Generic base for buffers
class ChannelBuffer : public std::streambuf,
    private NonCopyable {

private: // methods

    /// constructor, taking ownership of stream
    ChannelBuffer( std::size_t size = 1024 );

    /// destructor, deallocates stream if has ownership
    virtual ~ChannelBuffer();

    bool active() const { return target_ != 0; }

    void clear();
    void setTarget(LogTarget* target);

    void indent(const char* space = "   ");
    void unindent();

    void setCallback(channel_callback_t cb, void* data = 0);
    void addCallback(channel_callback_t cb, void* data = 0);

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

    LogTarget* target_;

    std::vector<char> buffer_;

private:


    friend class Channel;

};

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
