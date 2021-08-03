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
#include "eckit/thread/AutoLock.h"


namespace eckit {
namespace net {


TCPServer::TCPServer(const SocketOptions& options) :
    TCPSocket(), port_(0), listen_(-1), options_(options), closeExec_(true) {}

TCPServer::TCPServer(int port, const SocketOptions& options) :
    TCPSocket(), port_(port), listen_(-1), options_(options), closeExec_(true) {}

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
        Log::status() << message;
        if (port_) {
            Log::status() << " (port " << port_ << ")";
        }
        Log::status() << std::endl;

        while (!select.ready(delay)) {

            if (timeout && !connected) {
                throw TimeOut(message, timeout);
            }

            if (connected) {
                *connected = false;
                return *this;
            }

            Log::status() << message;
            if (port_) {
                Log::status() << " (port " << port_ << ")";
            }
            Log::status() << std::endl;
        }

        if ((socket_ = ::accept(listen_, reinterpret_cast<sockaddr*>(&from), &fromlen)) >= 0) {
            break;
        }

        if (errno != EINTR)
            throw FailedSystemCall("accept");
    }

    remoteAddr_ = from.sin_addr;
    remoteHost_ = addrToHost(from.sin_addr);
    remotePort_ = ntohs(from.sin_port);

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
    // There can be a race condition is two thread asks for localPort()
    // and both try to bind at the same time
    AutoLock<Mutex> lock(mutex_);

    if (listen_ == -1) {
        listen_     = createSocket(port_, options_);
        int backlog = options_.listenBacklog();
        Log::info() << "Listening on socket " << listen_ << " port: " << port_ << " backlog: " << backlog << std::endl;
        SYSCALL(::listen(listen_, backlog));
    }
}


int TCPServer::socket() {
    bind();
    return listen_;
}

std::string TCPServer::bindingAddress() const {
    return options_.bindAddress();
}

void TCPServer::print(std::ostream& s) const {
    s << "TCPServer["
      << "port=" << port_ << ",options_=" << options_ << ",";
    TCPSocket::print(s);
    s << "]";
}


EphemeralTCPServer::EphemeralTCPServer(const SocketOptions& opts) :
    TCPServer(0, opts) {}

EphemeralTCPServer::EphemeralTCPServer(int port, const SocketOptions& opts) :
    TCPServer(port, opts) {}


}  // namespace net
}  // namespace eckit
