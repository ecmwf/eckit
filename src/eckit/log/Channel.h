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

class ChannelBuffer;
class LogTarget;

typedef void (*channel_callback_t)(void* data, const char* msg);

//----------------------------------------------------------------------------------------------------------------------

/// @brief Abstract output stream interface
class Channel: public std::ostream, private NonCopyable {
public:  // methods
    virtual ~Channel() = default;

    virtual void indent(const char* prefix) { }
    virtual void unindent() { }

    virtual void setStream(std::ostream&) { }
    virtual void addStream(std::ostream&) { }

    virtual void setFile(const std::string& path) { }
    virtual void addFile(const std::string& path) { }

    virtual void setCallback(channel_callback_t cb, void* data = 0) { }
    virtual void addCallback(channel_callback_t cb, void* data = 0) { }

    virtual void setTarget(LogTarget*) { }
    virtual void addTarget(LogTarget*) { }

    virtual void reset() { }

protected:  // methods
    Channel(std::streambuf* buffer): std::ostream(buffer) { }

private:  // members
    virtual void print(std::ostream& s) const = 0;

    friend std::ostream& operator<<(std::ostream& os, const Channel& c) {
        c.print(os);
        return os;
    }
};

//----------------------------------------------------------------------------------------------------------------------

/// Output channel that is an std::ostream but more functional

class OutputChannel: public Channel {
public:  // methods
    OutputChannel(LogTarget* = 0);

    ~OutputChannel() override;

    bool operator!() const;
    operator bool() const;

    void indent(const char* space = "") override;  // not sure about default on override methods
    void unindent() override;

    void setStream(std::ostream& out) override;
    void addStream(std::ostream& out) override;

    void setFile(const std::string& path) override;
    void addFile(const std::string& path) override;

    void setCallback(channel_callback_t cb, void* data = 0) override;
    void addCallback(channel_callback_t cb, void* data = 0) override;

    void setTarget(LogTarget*) override;
    void addTarget(LogTarget*) override;

    void reset() override;

private:  // members
    void print(std::ostream& s) const override;

    ChannelBuffer* buffer_ {nullptr};

    friend class Log;
};

//----------------------------------------------------------------------------------------------------------------------


class AutoIndent {
    OutputChannel& channel_;

public:
    AutoIndent(OutputChannel& channel, const char* prefix = ""): channel_(channel) { channel_.indent(prefix); }
    ~AutoIndent() { channel_.unindent(); }
};

//----------------------------------------------------------------------------------------------------------------------

class EmptyChannel: public Channel {
public:  // methods
    EmptyChannel(): Channel(nullptr) { }

    ~EmptyChannel() override = default;

    bool operator!() const { return true; }
    operator bool() const { return false; }

private:  // members
    void print(std::ostream& s) const override { s << "EmptyChannel()"; }

    friend class Log;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
