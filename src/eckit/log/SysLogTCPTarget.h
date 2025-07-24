/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file   SysLogTCPTarget.h
/// @author Metin Cakircali
/// @date   July 2025

#include "eckit/log/SysLog.h"
#include "eckit/log/TCPTarget.h"

namespace eckit {

namespace net {
class TCPSocket;
}

//----------------------------------------------------------------------------------------------------------------------

/// sends SysLog (RFC 5424 format) logs over TCP
class SysLogTCPTarget : public TCPTarget {

    /// Example SysLog:
    /// SysLog log(SysLog::Local7, SysLog::Info);
    /// log.enterpriseId("7464");  // ECMWF's id
    /// log.appName("mars-client-cpp");
    /// log.software("mars");
    /// log.swVersion("0.0.1");

public:  // methods

    /// @param socket TCP socket to use for log messages
    /// @param log SysLog object that hold application specific information
    /// @note takes ownership of [socket]
    explicit SysLogTCPTarget(net::TCPSocket& socket, SysLog log);

    // rules
    SysLogTCPTarget(const SysLogTCPTarget&)            = delete;
    SysLogTCPTarget& operator=(const SysLogTCPTarget&) = delete;
    SysLogTCPTarget(SysLogTCPTarget&&)                 = delete;
    SysLogTCPTarget& operator=(SysLogTCPTarget&&)      = delete;

    ~SysLogTCPTarget() override = default;

    void write(const char* start, const char* end) override;

    void flush() override;

    void msgid(int msg_id);

private:  // members

    SysLog log_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
