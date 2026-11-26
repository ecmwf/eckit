// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Jun 96

#ifndef eckit_TCPClient_h
#define eckit_TCPClient_h

#include "eckit/net/TCPSocket.h"

namespace eckit::net {

class Endpoint;

class TCPClient : public TCPSocket {
public:

    TCPClient(const SocketOptions& options = SocketOptions::none());
    TCPClient(int port, const SocketOptions& options = SocketOptions::none());

    TCPClient(TCPClient&)             = delete;
    TCPClient& operator=(TCPClient&)  = delete;
    TCPClient(TCPClient&&)            = delete;
    TCPClient& operator=(TCPClient&&) = delete;

    ~TCPClient();

    virtual TCPSocket& connect(const std::string& host, int port, int retries = 5, int timeout = 0, int retryDelay = 5);
    virtual TCPSocket& connect(const net::Endpoint& endpoint, int retries = 5, int timeout = 0, int retryDelay = 5);

protected:  // methods

    void print(std::ostream& s) const override;

    void buildSockAddress();

private:  // members

    int port_;
    SocketOptions options_;

private:  // methods

    void bind() override;
    std::string bindingAddress() const override { return options_.bindAddress(); }
};

}  // namespace eckit::net

#endif  // TCPClient_H
