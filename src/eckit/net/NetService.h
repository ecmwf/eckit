// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @date   Jun 96

#ifndef eckit_NetService_h
#define eckit_NetService_h

#include <functional>
#include <optional>

#include "eckit/net/TCPServer.h"
#include "eckit/thread/Thread.h"

namespace eckit {
class ConcurrencyLimiter;
}

namespace eckit::net {

class NetUser;

class NetService : public Thread {

public:

    /// @param[in]  port     TCP port to listen on
    /// @param[in]  visible  Make the thread this service is running in visible on the Monitor (defaults to false)
    NetService(int port, bool visible = true, const SocketOptions& options = SocketOptions::server());

    /// @param[in]  limiter  Limits the number of connections served concurrently. It may be shared between several
    ///                      services, and must outlive them. Connections beyond the limit are refused immediately.
    NetService(int port, ConcurrencyLimiter& limiter, bool visible = true,
               const SocketOptions& options = SocketOptions::server());

    ~NetService();

    /// @returns hostname to which this server is answering
    std::string hostname() const;
    /// @returns port to which this server is answering
    int port() const;

    void run() override;

private:

    TCPServer server_;
    bool visible_;  ///< Visible on the Monitor?

    std::optional<std::reference_wrapper<ConcurrencyLimiter>> limiter_;

private:

    virtual NetUser* newUser(net::TCPSocket&) const = 0;
    virtual std::string name() const                = 0;

    virtual bool preferToRunAsProcess() const;
    virtual bool runAsProcess() const;

    virtual long timeout() const;

    /// Serve a connection if the limiter has a free slot, otherwise refuse it
    void serveLimited(NetUser*, ConcurrencyLimiter&);
};

}  // namespace eckit::net

#endif
