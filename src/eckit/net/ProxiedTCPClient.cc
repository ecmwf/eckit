// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/net/ProxiedTCPClient.h"

#include "eckit/net/HttpHeader.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ProxiedTCPClient::ProxiedTCPClient(const std::string& proxyHost, int proxyPort, int port) :
    TCPClient(port), proxy_(proxyHost, proxyPort) {}

ProxiedTCPClient::~ProxiedTCPClient() {}

net::TCPSocket& ProxiedTCPClient::connect(const std::string& host, int port, int retries, int timeout, int retryDelay) {
    net::TCPSocket& socket = TCPClient::connect(proxy_.hostname(), proxy_.port(), retries, timeout, retryDelay);

    socket.debug(debug_.on);

    const char* CRLF = "\r\n";

    std::ostringstream oss;
    oss << "CONNECT " << host << ":" << port << " HTTP/1.0" << CRLF;
    oss << "User-agent: eckit/1.0" << CRLF;
    oss << CRLF;

    std::string request(oss.str());
    auto len = long(request.size());
    ASSERT(socket.write(&request[0], len) == len);


    // Strip http-header
    HttpHeader header(socket);
    std::cout << std::endl << header << std::endl << std::endl;
    header.checkForStatus();

    return socket;
}

void ProxiedTCPClient::print(std::ostream& s) const {
    s << "ProxiedTCPClient["
      << "proxy=" << proxy_ << ",";
    TCPClient::print(s);
    s << "]";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
