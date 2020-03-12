/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ProxiedTCPServer.h
// Baudouin Raoult - ECMWF Aug 2017

#ifndef eckit_ProxiedTCPServer_h
#define eckit_ProxiedTCPServer_h

#include "eckit/net/TCPServer.h"


namespace eckit {

class ProxiedTCPServer : public net::TCPServer {
public:

    ProxiedTCPServer(int port = 0, const net::SocketOptions& = net::SocketOptions::server());

    ~ProxiedTCPServer();

    // accept a client, more can be accepted

    virtual net::TCPSocket& accept(const std::string& message = "Waiting for connection", int timeout = 0, bool* connected = nullptr);

private:

// No copy allowed

    ProxiedTCPServer(const ProxiedTCPServer&);
    ProxiedTCPServer& operator=(const ProxiedTCPServer&);

    //
    virtual void print(std::ostream& s) const;

};


} // namespace eckit

#endif
