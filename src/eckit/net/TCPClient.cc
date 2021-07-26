/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/net/TCPClient.h"
#include "eckit/config/Resource.h"
#include "eckit/net/Endpoint.h"


namespace eckit {
namespace net {


TCPClient::TCPClient(const SocketOptions& options) :
    TCPSocket(), port_(0), options_(options) {}
TCPClient::TCPClient(int port, const SocketOptions& options) :
    TCPSocket(), port_(port), options_(options) {}

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
TCPSocket& TCPClient::connect(const net::Endpoint& endpoint, int retries, int timeout) {
    return connect(endpoint.hostname(), endpoint.port(), retries, timeout);
}

}  // namespace net
}  // namespace eckit
