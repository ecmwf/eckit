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
/// @date   Jun 96

#ifndef eckit_TCPClient_h
#define eckit_TCPClient_h

#include "eckit/net/TCPSocket.h"

namespace eckit::net {

class Endpoint;

class TCPClient : public TCPSocket, private eckit::NonCopyable {
public:

    TCPClient(const SocketOptions& options = SocketOptions::none());
    TCPClient(int port, const SocketOptions& options = SocketOptions::none());

    ~TCPClient();

    virtual TCPSocket& connect(const std::string& host, int port, int retries = 5, int timeout = 0);
    virtual TCPSocket& connect(const net::Endpoint& endpoint, int retries = 5, int timeout = 0);

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
