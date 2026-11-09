// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef eckit_NetUser_h
#define eckit_NetUser_h

#include "eckit/net/TCPSocket.h"
#include "eckit/thread/Thread.h"


namespace eckit {

class Stream;

namespace net {

class NetUser : public Thread {
public:

    NetUser(net::TCPSocket&);

    ~NetUser();

protected:

    TCPSocket protocol_;

private:

    virtual void serve(Stream&, std::istream&, std::ostream&) = 0;

    /// Called instead of serve() when the connection cannot be served (e.g. the server is busy).
    /// Override to report the reason to the client in a protocol-specific way. The default only logs.
    virtual void refuse(Stream&, std::istream&, std::ostream&, const std::string& reason);

    void run() override;

    void reject(const std::string& reason);

    friend class NetService;
    friend class NetServiceProcessControler;
    friend class NetServiceWorker;
    friend class NetServiceChildWaiter;
};


}  // namespace net
}  // namespace eckit

#endif
