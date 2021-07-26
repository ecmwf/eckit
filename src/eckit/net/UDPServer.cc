/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/Buffer.h"
#include "eckit/log/Bytes.h"
#include "eckit/net/UDPServer.h"
#include "eckit/utils/Translator.h"

namespace eckit {
namespace net {


static void* get_sockaddr(struct sockaddr* sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    else
        return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


UDPServer::UDPServer(int port) :
    port_(port), socketfd_(0) {

    struct addrinfo hints;
    struct addrinfo* servinfo;
    struct addrinfo* addr;

    ::memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags    = AI_PASSIVE;  // use local IP

    Translator<int, std::string> toStr;

    int err = 0;
    if ((err = ::getaddrinfo(NULL, toStr(port_).c_str(), &hints, &servinfo)) != 0) {
        std::ostringstream msg;
        msg << "getaddrinfo failed in UDPServer with "
            << " port=" << port << " --  " << ::gai_strerror(err);
        throw FailedSystemCall(msg.str(), Here());
    }

    // loop through all the addrinfo results and make a socket and then bind()
    for (addr = servinfo; addr != NULL; addr = addr->ai_next) {
        if ((socketfd_ = ::socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol)) == -1) {
            continue;
        }

        if (::bind(socketfd_, addr->ai_addr, addr->ai_addrlen) == -1) {
            ::close(socketfd_);
            Log::warning() << "UPDServer failed to bind() to socket " << socketfd_ << std::endl;
            continue;
        }

        break;
    }

    if (addr == NULL) {
        std::ostringstream msg;
        msg << "UDPServer failed to create a socket";
        throw FailedSystemCall(msg.str(), Here());
    }

    ::freeaddrinfo(servinfo);

    Log::info() << *this << std::endl;
}

UDPServer::~UDPServer() {
    SYSCALL(::close(socketfd_));
}

size_t UDPServer::receive(void* buffer, long length) {
    struct sockaddr_storage remote_addr;
    socklen_t addr_len = sizeof(remote_addr);

    Log::info() << "UDPServer waiting on recvfrom()" << std::endl;

    ssize_t received = 0;
    if ((received = ::recvfrom(socketfd_, buffer, length, 0, (struct sockaddr*)&remote_addr, &addr_len)) == -1) {
        std::ostringstream msg;
        msg << "UDPServer port " << port_ << " error on recvfrom socket " << socketfd_;
        throw FailedSystemCall(msg.str(), Here());
    }

    Log::info() << "Received messaged from " << remoteHost(remote_addr) << std::endl;

    return received;
}

size_t UDPServer::receive(eckit::Buffer& buffer) {
    return receive(buffer, buffer.size());
}

void UDPServer::print(std::ostream& s) const {
    s << "UDPServer[port=" << port_ << ",socketfd=" << socketfd_ << "]";
}

std::string UDPServer::remoteHost(struct sockaddr_storage& remote_addr) const {
    char inet6[INET6_ADDRSTRLEN];
    std::string r = ::inet_ntop(remote_addr.ss_family, get_sockaddr((struct sockaddr*)&remote_addr), inet6, sizeof(inet6));
    return r;
}


}  // namespace net
}  // namespace eckit
