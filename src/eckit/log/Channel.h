/*
 * (C) Copyright 1996-2016 ECMWF.
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

//----------------------------------------------------------------------------------------------------------------------

/// Output channel that is an std::ostream but more functional
/// @todo consider merging Channel with MultiChannel
class Channel : public std::ostream, private NonCopyable {

public: // methods

     Channel(LogTarget* = 0);
    ~Channel();

    bool operator !() const;
    operator bool() const;

    void setLogTarget(LogTarget*);
    void addLogTarget(LogTarget*);
    void clearLogTargets();

    void indent(const char* prefix = "");
    void unindent();

private:

    ChannelBuffer* buffer_;

};


class AutoIndent {
    Channel& channel_;
public:
    AutoIndent(Channel& channel, const char* prefix = "") : channel_(channel) { channel_.indent(prefix); }
    ~AutoIndent() { channel_.unindent(); }
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
