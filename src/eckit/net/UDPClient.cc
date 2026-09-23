// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/net/UDPClient.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "eckit/config/Configuration.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Bytes.h"
#include "eckit/utils/Translator.h"

namespace eckit::net {

UDPClient::UDPClient(const Configuration& cfg) :
    hostname_(cfg.getString("host")), port_(cfg.getInt("port")), socketfd_(0), servinfo_{nullptr}, addr_{nullptr} {
    init();
}

UDPClient::UDPClient(const std::string& hostname, int port) :
    hostname_(hostname), port_(port), socketfd_(0), servinfo_{nullptr}, addr_{nullptr} {
    init();
}

void UDPClient::init() {
    struct addrinfo hints;

    ::memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    Translator<int, std::string> toStr;

    int err = 0;
    if ((err = ::getaddrinfo(hostname_.c_str(), toStr(port_).c_str(), &hints, &servinfo_)) != 0) {
        std::ostringstream msg;
        msg << "getaddrinfo failed in UDPClient with "
            << " hostname=" << hostname_ << " port=" << port_ << " --  " << ::gai_strerror(err);
        throw FailedSystemCall(msg.str());
    }

    // loop through all the addrinfo results and make a socket
    for (addr_ = servinfo_; addr_ != nullptr; addr_ = addr_->ai_next) {
        if ((socketfd_ = ::socket(addr_->ai_family, addr_->ai_socktype, addr_->ai_protocol)) == -1) {
            continue;
        }
        break;
    }

    if (addr_ == nullptr) {
        std::ostringstream msg;
        msg << "UDPClient failed to create a socket";
        throw FailedSystemCall(msg.str());
    }
}

UDPClient::~UDPClient() {
    ::freeaddrinfo(servinfo_);
    SYSCALL(::close(socketfd_));
}

void UDPClient::send(const void* buffer, size_t length) {
    ssize_t sent = ::sendto(socketfd_, buffer, length, 0, addr_->ai_addr, addr_->ai_addrlen);
    if (sent == -1) {
        std::ostringstream msg;
        msg << "UDPClient failed to send " << Bytes(length) << " to host " << hostname_;
        throw FailedSystemCall(msg.str());
    }
}

void UDPClient::print(std::ostream& s) const {
    s << "UDPClient[hostname=" << hostname_ << ",port=" << port_ << ",socketfd=" << socketfd_ << "]";
}

}  // namespace eckit::net
