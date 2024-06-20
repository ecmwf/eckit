/*
 * (C) Copyright 1996- ECMWF.
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

#include <ostream>

#include "eckit/memory/NonCopyable.h"

namespace eckit {

class VoidBuffer;
class ChannelBuffer;
class LogTarget;

typedef void (*channel_callback_t)(void* data, const char* msg);

//----------------------------------------------------------------------------------------------------------------------

/// Output channel that is an std::ostream but more functional

class Channel: public std::ostream, private NonCopyable {
public:  // methods
    Channel(LogTarget* = 0);

    virtual ~Channel();

    bool operator!() const;
    operator bool() const;

    void indent(const char* space = "");
    void unindent();

    void setStream(std::ostream& out);
    void addStream(std::ostream& out);

    void setFile(const std::string& path);
    void addFile(const std::string& path);

    void setCallback(channel_callback_t cb, void* data = 0);
    void addCallback(channel_callback_t cb, void* data = 0);

    void setTarget(LogTarget* target);
    void addTarget(LogTarget* target);

    void reset();

protected:  // methods
    explicit Channel(VoidBuffer* dummy);

private:  // methods
    explicit Channel(ChannelBuffer* buffer);

    virtual void print(std::ostream& s) const;

private:  // members
    ChannelBuffer* buffer_ {nullptr};

    friend class Log;
};

//----------------------------------------------------------------------------------------------------------------------

class AutoIndent {
    Channel& channel_;

public:
    AutoIndent(Channel& channel, const char* prefix = ""): channel_(channel) { channel_.indent(prefix); }
    ~AutoIndent() { channel_.unindent(); }
};

//----------------------------------------------------------------------------------------------------------------------

class EmptyChannel: public Channel {
public:  // methods
    EmptyChannel();

    ~EmptyChannel() = default;

private:  // methods
    void print(std::ostream& s) const override { s << "EmptyChannel()"; }

    friend class Log;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
