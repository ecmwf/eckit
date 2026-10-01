// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/log/TCPTarget.h"

#include <ostream>
#include <string>

#include "eckit/net/TCPSocket.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

TCPTarget::TCPTarget(net::TCPSocket& socket) : out_(socket) {}

void TCPTarget::write(const char* start, const char* end) {
    if (start && end && end >= start) {
        out_.write(static_cast<const void*>(start), end - start);
    }
}

void TCPTarget::write(const std::string& msg) {
    if (!msg.empty()) {
        out_.write(msg.data(), msg.size());
    }
}

void TCPTarget::flush() {
    // no-op
}

void TCPTarget::print(std::ostream& out) const {
    out << "TCPTarget(" << out_ << ")";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
