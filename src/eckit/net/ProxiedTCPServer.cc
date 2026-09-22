// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/net/ProxiedTCPServer.h"

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <csignal>

#include "eckit/io/Select.h"
#include "eckit/log/Log.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ProxiedTCPServer::ProxiedTCPServer(int port, const net::SocketOptions& options) : net::TCPServer(port, options) {}

ProxiedTCPServer::~ProxiedTCPServer() {}

// Accept a client

net::TCPSocket& ProxiedTCPServer::accept(const std::string& message, int timeout, bool* connected) {
    net::TCPSocket& socket = TCPServer::accept(message, timeout, connected);

    // Strip http-header

    char c;
    unsigned long x   = 0;
    unsigned long end = ('\r' << 24L) | ('\n' << 16L) | ('\r' << 8L) | '\n';

    while (socket.read(&c, 1) == 1) {

        x <<= 8L;
        x |= c;
        x &= 0xffffffff;

        if (x == end) {
            return socket;
        }
    }

    throw SeriousBug("ProxiedTCPServer: invalid header");
}


void ProxiedTCPServer::print(std::ostream& s) const {
    s << "ProxiedTCPServer[";
    TCPServer::print(s);
    s << "]";
}
//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
