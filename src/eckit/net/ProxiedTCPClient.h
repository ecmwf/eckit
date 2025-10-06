/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Aug 2017

#ifndef eckit_ProxiedTCPClient_h
#define eckit_ProxiedTCPClient_h

#include "eckit/net/Endpoint.h"
#include "eckit/net/TCPClient.h"

namespace eckit {


class ProxiedTCPClient : public net::TCPClient {
public:

    ProxiedTCPClient(const std::string& proxyHost, int proxyPort, int port = 0);

    ~ProxiedTCPClient() override;

    using net::TCPClient::connect;
    net::TCPSocket& connect(const std::string& host, int port, int retries = 5, int timeout = 0) override;

private:

    net::Endpoint proxy_;

    void print(std::ostream& s) const override;
};


}  // namespace eckit

#endif
