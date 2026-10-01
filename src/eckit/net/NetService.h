// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @date   Jun 96

#ifndef eckit_NetService_h
#define eckit_NetService_h

#include "eckit/net/TCPServer.h"
#include "eckit/thread/Thread.h"

namespace eckit::net {

class NetUser;

class NetService : public Thread {

public:

    /// @param[in]  port     TCP port to listen on
    /// @param[in]  visible  Make the thread this service is running in visible on the Monitor (defaults to false)
    NetService(int port, bool visible = true, const SocketOptions& options = SocketOptions::server());

    ~NetService();

    /// @returns hostname to which this server is answering
    std::string hostname() const;
    /// @returns port to which this server is answering
    int port() const;

    void run() override;

private:

    TCPServer server_;
    bool visible_;  ///< Visible on the Monitor?

private:

    virtual NetUser* newUser(net::TCPSocket&) const = 0;
    virtual std::string name() const                = 0;

    virtual bool preferToRunAsProcess() const;
    virtual bool runAsProcess() const;

    virtual long timeout() const;
};

}  // namespace eckit::net

#endif
