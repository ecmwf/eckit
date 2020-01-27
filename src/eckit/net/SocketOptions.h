/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_net_SocketOptions_h
#define eckit_net_SocketOptions_h

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Jan 2020

#include <string>
#include <iosfwd>

namespace eckit {
namespace net {

struct SocketOptions {

    SocketOptions();

    SocketOptions& reusePort(bool v = true) {
        reusePort_ = v;
        return *this;
    }

    SocketOptions& reuseAddr(bool v = true) {
        reuseAddr_ = v;
        return *this;
    }

    SocketOptions& noLinger(bool v = true) {
        noLinger_ = v;
        return *this;
    }

    SocketOptions& keepAlive(bool v = true) {
        keepAlive_ = v;
        return *this;
    }

    SocketOptions& ipLowDelay(bool v = true) {
        ipLowDelay_ = v;
        return *this;
    }

    SocketOptions& tcpNoDelay(bool v = true) {
        tcpNoDelay_ = v;
        return *this;
    }

    SocketOptions& bindAddress(const std::string& addr) {
        bindAddr_ = addr;
        return *this;
    }

    bool reusePort() const { return reusePort_; }
    bool reuseAddr() const { return reuseAddr_; }
    bool keepAlive() const { return keepAlive_; }
    bool noLinger() const { return noLinger_; }
    bool ipLowDelay() const { return ipLowDelay_; }
    bool tcpNoDelay() const { return tcpNoDelay_; }

    std::string bindAddress() const { return bindAddr_; }

    void print(std::ostream& s) const;

private:
    /// Binding address for this socket
    std::string bindAddr_ = "";

    /// SO_REUSEPORT is useful if multiple threads want to bind to the same port and OS handles load balancing
    /// otherwise better not to set it.
    bool reusePort_ = false;

    /// SO_REUSEADDRESS tells OS to skip TIME_WAIT of 2MSL before rebinding to same address:port
    /// This allows fast restart of services, at the added risk of duplicated/stray packets in route
    /// from previous closed connections messing up new connections. This is likely under high network contention and/or
    /// very short connections quickly reuse the available ports.
    /// Set to false when openeing ephemeral ports/connections.
    bool reuseAddr_ = true;

    /// Enable sending of keep-alive messages
    bool keepAlive_ = true;

    /// Do the close in the background (don't wait for queued messages) and return immedietly on close()
    bool noLinger_ = true;

    /// sets IP_TOS to IPTOS_LOWDELAY to minimize delays for interactive traffic (small messages) as per manpage ip(7)
    bool ipLowDelay_ = true;

    /// bypass Nagle Delays by disabling Nagle's algorithm and send the data as soon as it's available
    bool tcpNoDelay_ = true;

private:
    friend std::ostream& operator<<(std::ostream& s, const SocketOptions& socket);
};


//----------------------------------------------------------------------------------------------------------------------

/// Options set for server processes that need quick restarts
struct ServerSocket : public SocketOptions {
    ServerSocket();
};

/// Options set for ephemeral sockets transfering large buffers
struct DataSocket : public SocketOptions {
    DataSocket();
};


/// Options set for optimal nlow latency, small messages
struct ControlSocket : public SocketOptions {
    ControlSocket();
};


}  // namespace net
}  // namespace eckit

#endif
