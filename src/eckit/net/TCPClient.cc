// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/net/TCPClient.h"
#include "eckit/config/Resource.h"
#include "eckit/net/Endpoint.h"

namespace eckit::net {

TCPClient::TCPClient(const SocketOptions& options) : TCPSocket(), port_(0), options_(options) {}
TCPClient::TCPClient(int port, const SocketOptions& options) : TCPSocket(), port_(port), options_(options) {}

TCPClient::~TCPClient() {}

void TCPClient::bind() {
    if (socket_ == -1) {
        socket_ = createSocket(port_, options_);
    }
}

void TCPClient::print(std::ostream& s) const {
    s << "TCPClient["
      << "port=" << port_ << ",options=" << options_;
    TCPSocket::print(s);
    s << "]";
}

/// @note TCPClient::connect(host, port, retries, timeout) is found in TCPSocket.cc
TCPSocket& TCPClient::connect(const net::Endpoint& endpoint, int retries, int timeout, int retryDelay) {
    return connect(endpoint.hostname(), endpoint.port(), retries, timeout, retryDelay);
}

}  // namespace eckit::net
