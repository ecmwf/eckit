// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/net/TCPStream.h"

namespace eckit::net {

TCPStream::TCPStream(net::TCPSocket& socket) : socket_(socket) {}

TCPStream::~TCPStream() {}

void TCPStream::closeOutput() {
    socket_.closeOutput();
}
//----------------------------------------------------------------------------------------------------------------------
// Tricky solution to be removed when 'mutable' is available
//
std::string TCPStreamBase::nonConstName() {
    std::ostringstream r;
    r << "TCPStream[" << socket() << "]";
    return r.str();
}

std::string TCPStreamBase::name() const {
    return ((TCPStreamBase*)this)->nonConstName();
}

SharedTCPStream::SharedTCPStream(net::TCPSocket& s) : TCPStream(s) {}

SharedTCPStream::~SharedTCPStream() {}

}  // namespace eckit::net
