/*
 * (C) Copyright 1996-2017 ECMWF.
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

/// Stream buffer to be usedby Channel
class ChannelBuffer :
        public std::streambuf,
        private NonCopyable {

private: // methods

    /// constructor, taking ownership of stream
    ChannelBuffer( std::size_t size = 1024 );

    virtual ~ChannelBuffer();

    bool active() const;

    void reset();

    void setTarget(LogTarget* target);
    void addTarget(LogTarget* target);

    void indent(const char* space = "   ");
    void unindent();

    void setStream(std::ostream& out);
    void addStream(std::ostream& out);

    void setFile(const std::string& path);
    void addFile(const std::string& path);

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
