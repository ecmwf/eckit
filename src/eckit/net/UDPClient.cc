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

#include "eckit/config/Configuration.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Bytes.h"
#include "eckit/net/UDPClient.h"
#include "eckit/utils/Translator.h"

namespace eckit {
namespace net {

UDPClient::UDPClient(const Configuration& cfg) :
    hostname_(cfg.getString("host")), port_(cfg.getInt("port")), socketfd_(0), servinfo_(nullptr), addr_(nullptr) {
    init();
}

UDPClient::UDPClient(const std::string& hostname, int port) :
    hostname_(hostname), port_(port), socketfd_(0), servinfo_(nullptr), addr_(nullptr) {
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
    for (addr_ = servinfo_; addr_ != NULL; addr_ = addr_->ai_next) {
        if ((socketfd_ = ::socket(addr_->ai_family, addr_->ai_socktype, addr_->ai_protocol)) == -1) {
            continue;
        }
        break;
    }

    if (addr_ == NULL) {
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


}  // namespace net
}  // namespace eckit
