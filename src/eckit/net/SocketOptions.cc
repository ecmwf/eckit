/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <ostream>

#include "eckit/net/SocketOptions.h"
#include "eckit/config/Resource.h"

namespace eckit {
namespace net {

static void init(SocketOptions& opts) {
    static bool ReusePort  = eckit::Resource<bool>("socketOptionsReusePort",  false);
    static bool ReuseAddr  = eckit::Resource<bool>("socketOptionsReuseAddr",  false);
    static bool NoLinger   = eckit::Resource<bool>("socketOptionsNoLinger",   false);
    static bool KeepAlive  = eckit::Resource<bool>("socketOptionsKeepAlive",  false);
    static bool IpLowDelay = eckit::Resource<bool>("socketOptionsIpLowDelay", false);
    static bool TcpNoDelay = eckit::Resource<bool>("socketOptionsTcpNoDelay", false);

    opts.reusePort(ReusePort);
    opts.reuseAddr(ReuseAddr);
    opts.noLinger(NoLinger);
    opts.keepAlive(KeepAlive);
    opts.ipLowDelay(IpLowDelay);
    opts.tcpNoDelay(TcpNoDelay);
}


SocketOptions::SocketOptions() {
    init(*this);
}

void SocketOptions::print(std::ostream& s) const {
    s << "SocketOptions["
      << "bindAddr=" << bindAddr_ << ", "
      << "reusePort=" << reusePort_ << ", "
      << "reuseAddr=" << reuseAddr_ << ", "
      << "keepAlive=" << keepAlive_ << ", "
      << "noLinger=" << noLinger_ << ", "
      << "ipLowDelay=" << ipLowDelay_ << ", "
      << "tcpNoDelay=" << tcpNoDelay_ << "]" << std::endl;
}

std::ostream& operator<<(std::ostream& s, const SocketOptions& o) {
    o.print(s);
    return s;
}

ServerSocket::ServerSocket() {
    reuseAddr(true);
    keepAlive(true);
}

DataSocket::DataSocket() {
}

ControlSocket::ControlSocket() {
    keepAlive(true);
    ipLowDelay(true);
    tcpNoDelay(true);
}

}  // namespace net
}  // namespace eckit
