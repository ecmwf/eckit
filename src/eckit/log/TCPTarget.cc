/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
