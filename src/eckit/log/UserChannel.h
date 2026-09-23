// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file UserChannel.h
/// @author Tiago Quintino

#ifndef eckit_log_UserChannel_h
#define eckit_log_UserChannel_h

#include <ostream>
#include <string>

namespace eckit {

//-----------------------------------------------------------------------------

class UserMsg {
public:

    virtual ~UserMsg() {}
    virtual void infoMsg(const std::string&)      = 0;
    virtual void warningMsg(const std::string&)   = 0;
    virtual void errorMsg(const std::string&)     = 0;
    virtual void notifyClient(const std::string&) = 0;
};

//-----------------------------------------------------------------------------

class UserBuffer;

class UserChannel : public std::ostream {
public:  // types

    enum MsgType {
        NONE,
        INFO,
        ERROR,
        WARN
    };

public:  // methods

    /// Constructor
    UserChannel();

    UserChannel(const UserChannel&)            = delete;
    UserChannel& operator=(const UserChannel&) = delete;
    UserChannel(UserChannel&&)                 = delete;
    UserChannel& operator=(UserChannel&&)      = delete;

    /// Destructor
    ~UserChannel();

    /// type for next message
    void msgType(MsgType t);

    void userMsg(UserMsg*);
    UserMsg* userMsg() const;

protected:

    UserBuffer* buffer_;
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
