// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @date   Aug 2017

#ifndef eckit_ProxiedTCPServer_h
#define eckit_ProxiedTCPServer_h

#include "eckit/net/TCPServer.h"


namespace eckit {

class ProxiedTCPServer : public net::TCPServer {
public:

    ProxiedTCPServer(int port = 0, const net::SocketOptions& = net::SocketOptions::server());

    ~ProxiedTCPServer() override;

    // accept a client, more can be accepted

    virtual net::TCPSocket& accept(const std::string& message = "Waiting for connection", int timeout = 0,
                                   bool* connected = nullptr) override;

private:

    void print(std::ostream& s) const override;
};


}  // namespace eckit

#endif
