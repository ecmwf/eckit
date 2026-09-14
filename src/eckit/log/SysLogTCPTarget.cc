// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/log/SysLogTCPTarget.h"

#include <utility>

#include "eckit/log/SysLog.h"
#include "eckit/net/TCPSocket.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

SysLogTCPTarget::SysLogTCPTarget(net::TCPSocket& socket, SysLog log) : TCPTarget(socket), log_(std::move(log)) {}

void SysLogTCPTarget::msgid(int msg_id) {
    log_.msgid(msg_id);
}

void SysLogTCPTarget::write(const char* start, const char* end) {
    log_.message({start, end});
    TCPTarget::write(log_);
}

void SysLogTCPTarget::flush() {
    // no-op
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
