// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
