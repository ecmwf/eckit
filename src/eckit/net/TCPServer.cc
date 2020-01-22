/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>

#include "eckit/config/Resource.h"
#include "eckit/io/Select.h"
#include "eckit/log/Log.h"
#include "eckit/net/TCPServer.h"


namespace eckit {
namespace net {


TCPServer::TCPServer(int port, const std::string& addr, const SocketOptions socketOptions) :
    TCPSocket(), port_(port), listen_(-1), addr_(addr), socketOpts_(socketOptions), closeExec_(true) {}

TCPServer::~TCPServer() {
    if (listen_ >= 0) {
        ::close(listen_);
    }
}


TCPSocket& TCPServer::accept(const std::string& message, int timeout, bool* connected) {

    bind();

    sockaddr_in from;
    socklen_t fromlen = sizeof(from);

    for (;;) {
        int delay = timeout ? timeout : 10;

        Select select(listen_);
        Log::status() << message << " (port " << port_ << ")" << std::endl;

        while (!select.ready(delay)) {

            if (timeout && !connected) {
                throw TimeOut(message, timeout);
            }

            if (connected) {
                *connected = false;
                return *this;
            }

            Log::status() << message << " (port " << port_ << ")" << std::endl;
        }

        if ((socket_ = ::accept(listen_, reinterpret_cast<sockaddr*>(&from), &fromlen)) >= 0) {
            break;
        }

        if (errno != EINTR)
            throw FailedSystemCall("accept");
    }

    remoteAddr_ = from.sin_addr;
    remoteHost_ = addrToHost(from.sin_addr);
    remotePort_ = from.sin_port;

    // Set the 'close on exec'

    if (closeExec_)
        SYSCALL(fcntl(socket_, F_SETFD, FD_CLOEXEC));

    register_ignore_sigpipe();

    Log::status() << "Get connection from " << remoteHost() << std::endl;

    if (connected) {
        *connected = true;
    }

    return *this;
}

void TCPServer::close() {
    TCPSocket::close();
    if (listen_ >= 0)
        ::close(listen_);
    listen_ = -1;
}

void TCPServer::bind() {
    if (listen_ == -1) {
        listen_ = createSocket(port_, socketOpts_);
        ::listen(listen_, 5);
    }
}


int TCPServer::socket() {
    bind();
    return listen_;
}

std::string TCPServer::bindingAddress() const {
    return addr_;
}

void TCPServer::print(std::ostream& s) const {
    s << "TCPServer["
      << "port=" << port_ << ",addr=" << addr_ << ",";
    TCPSocket::print(s);
    s << "]";
}

void EphemeralTCPServer::init(SocketOptions& opts) {
    static bool ReusePort  = eckit::Resource<bool>("ephemeralTCPServerReusePort",  false);
    static bool ReuseAddr  = eckit::Resource<bool>("ephemeralTCPServerReuseAddr",  false);
    static bool NoLinger   = eckit::Resource<bool>("ephemeralTCPServerNoLinger",   false);
    static bool KeepAlive  = eckit::Resource<bool>("ephemeralTCPServerKeepAlive",  false);
    static bool IpLowDelay = eckit::Resource<bool>("ephemeralTCPServerIpLowDelay", false);
    static bool TcpNoDelay = eckit::Resource<bool>("ephemeralTCPServerTcpNoDelay", false);

    opts.reusePort(ReusePort);
    opts.reuseAddr(ReuseAddr);
    opts.noLinger(NoLinger);
    opts.keepAlive(KeepAlive);
    opts.ipLowDelay(IpLowDelay);
    opts.tcpNoDelay(TcpNoDelay);
}

EphemeralTCPServer::EphemeralTCPServer(const std::string& addr) : TCPServer(0, addr) {
    init(socketOpts_);
}

EphemeralTCPServer::EphemeralTCPServer(int port, const std::string& addr) : TCPServer(port, addr) {
    init(socketOpts_);
}


}  // namespace net
}  // namespace eckit
