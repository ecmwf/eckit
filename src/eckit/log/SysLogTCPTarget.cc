/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
