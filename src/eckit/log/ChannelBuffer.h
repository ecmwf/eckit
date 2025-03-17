/*
 * (C) Copyright 1996- ECMWF.
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

#include <streambuf>
#include <vector>

#include "eckit/log/Channel.h"
#include "eckit/memory/NonCopyable.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class LogTarget;

/// Stream buffer to be usedby Channel
class ChannelBuffer : public std::streambuf, private NonCopyable {

private:  // methods

    /// constructor, taking ownership of stream
    ChannelBuffer(std::size_t size = 1024);

    ~ChannelBuffer() override;

    bool active() const;

    void reset();

    void setTarget(LogTarget* target);
    void addTarget(LogTarget* target);

    void indent(const char* space = "   ");
    void unindent();

    void setStream(std::ostream& out);
    void addStream(std::ostream& out);

    void setFile(const std::string& path, size_t bufferSize = 4 * 1024);
    void addFile(const std::string& path, size_t bufferSize = 4 * 1024);

    void setCallback(channel_callback_t cb, void* data = 0);
    void addCallback(channel_callback_t cb, void* data = 0);

protected:  // methods

    /// override this to change buffer behavior
    /// @returns true if no error occured
    virtual bool dumpBuffer();

    /// typically you don't need to override this
    /// @see dumpBuffer
    int_type overflow(int_type ch) override;

    /// typically you don't need to override this
    /// @see dumpBuffer
    int_type sync() override;

protected:  // members

    LogTarget* target_;

    std::vector<char> buffer_;

private:

    friend std::ostream& operator<<(std::ostream& os, const ChannelBuffer& c) {
        c.print(os);
        return os;
    }

    void print(std::ostream& s) const;

    friend class Channel;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
