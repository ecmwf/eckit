// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
    net::TCPSocket& connect(const std::string& host, int port, int retries = 5, int timeout = 0,
                            int retryDelay = 5) override;

private:

    net::Endpoint proxy_;

    void print(std::ostream& s) const override;
};


}  // namespace eckit

#endif
