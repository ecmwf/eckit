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

#include <iosfwd>
#include <string>

namespace eckit::net {

struct SocketOptions {
    static SocketOptions none();
    static SocketOptions server();
    static SocketOptions control();
    static SocketOptions data();


    SocketOptions& bindAddress(const std::string& addr) {
        bindAddr_ = addr;
        return *this;
    }

    SocketOptions& listenBacklog(int backlog) {
        listenBacklog_ = backlog;
        return *this;
    }

    SocketOptions& receiveBufferSize(int size) {
        receiveBufferSize_ = size;
        return *this;
    }

    SocketOptions& sendBufferSize(int size) {
        sendBufferSize_ = size;
        return *this;
    }

    SocketOptions& reusePort(bool v) {
        reusePort_ = v;
        return *this;
    }

    SocketOptions& reuseAddr(bool v) {
        reuseAddr_ = v;
        return *this;
    }

    SocketOptions& noLinger(bool v) {
        noLinger_ = v;
        return *this;
    }

    SocketOptions& keepAlive(bool v) {
        keepAlive_ = v;
        return *this;
    }

    SocketOptions& ipLowDelay(bool v) {
        ipLowDelay_ = v;
        return *this;
    }

    SocketOptions& tcpNoDelay(bool v) {
        tcpNoDelay_ = v;
        return *this;
    }

    bool reusePort() const { return reusePort_; }
    bool reuseAddr() const { return reuseAddr_; }
    bool keepAlive() const { return keepAlive_; }
    bool noLinger() const { return noLinger_; }
    bool ipLowDelay() const { return ipLowDelay_; }
    bool tcpNoDelay() const { return tcpNoDelay_; }

    int listenBacklog() const { return listenBacklog_; }
    int receiveBufferSize() const { return receiveBufferSize_; }
    int sendBufferSize() const { return sendBufferSize_; }

    std::string bindAddress() const { return bindAddr_; }

    void print(std::ostream& s) const;

private:

    SocketOptions();

    friend std::ostream& operator<<(std::ostream& s, const SocketOptions& socket);

private:

    /// Binding address for this socket
    std::string bindAddr_ = "";

    /// Value to pass as backlog to ::listen() sockets
    int listenBacklog_ = 5;

    /// size in bytes of the receive buffer
    int receiveBufferSize_ = 0;

    /// size in bytes of the send buffer
    int sendBufferSize_ = 0;

    /// SO_REUSEPORT is useful if multiple threads want to bind to the same port and OS handles load balancing
    /// otherwise better not to set it.
    bool reusePort_ = false;

    /// SO_REUSEADDRESS tells OS to skip TIME_WAIT of 2MSL before rebinding to same address:port
    /// This allows fast restart of services, at the added risk of duplicated/stray packets in route
    /// from previous closed connections messing up new connections. This is likely under high network contention and/or
    /// very short connections quickly reuse the available ports.
    /// Set to false when openeing ephemeral ports/connections.
    bool reuseAddr_ = false;

    /// Enable sending of keep-alive messages
    bool keepAlive_ = false;

    /// Do the close in the background (don't wait for queued messages) and return immedietly on close()
    bool noLinger_ = false;

    /// sets IP_TOS to IPTOS_LOWDELAY to minimize delays for interactive traffic (small messages) as per manpage ip(7)
    bool ipLowDelay_ = false;

    /// bypass Nagle Delays by disabling Nagle's algorithm and send the data as soon as it's available
    bool tcpNoDelay_ = false;
};

}  // namespace eckit::net

#endif
