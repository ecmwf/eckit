/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/net/ProxiedTCPClient.h"

#include "eckit/config/Resource.h"
#include "eckit/net/HttpHeader.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ProxiedTCPClient::ProxiedTCPClient(const std::string& proxyHost, int proxyPort, int port) :
    TCPClient(port), proxy_(proxyHost, proxyPort) {}

ProxiedTCPClient::~ProxiedTCPClient() {}

net::TCPSocket& ProxiedTCPClient::connect(const std::string& host, int port, int retries, int timeout) {
    net::TCPSocket& socket = TCPClient::connect(proxy_.hostname(), proxy_.port(), retries, timeout);

    socket.debug(debug_);

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
    std::cout << std::endl
              << header << std::endl
              << std::endl;
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
