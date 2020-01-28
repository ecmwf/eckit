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

    static std::string bindAddrAlias = Resource<std::string>("localBindingAddress", ""); /* "127.0.0.1" */
    static std::string bindAddr = Resource<std::string>("localBindingAddr", bindAddrAlias);

    opts.bindAddress(bindAddr);

    static int ListenBacklog = eckit::Resource<int>("socketOptionsListenBacklog", 5);

    opts.listenBacklog(ListenBacklog);

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
      << "listenBacklog=" << listenBacklog_ << ", "
      << "reusePort=" << reusePort_ << ", "
      << "reuseAddr=" << reuseAddr_ << ", "
      << "keepAlive=" << keepAlive_ << ", "
      << "noLinger=" << noLinger_ << ", "
      << "ipLowDelay=" << ipLowDelay_ << ", "
      << "tcpNoDelay=" << tcpNoDelay_ << "]" << std::endl;
}

SocketOptions SocketOptions::none() {
    return SocketOptions();
}

SocketOptions SocketOptions::server() {
    return SocketOptions().reuseAddr(true).keepAlive(true);
}

SocketOptions SocketOptions::control() {
    return SocketOptions().keepAlive(true).ipLowDelay(true).tcpNoDelay(true);
}

SocketOptions SocketOptions::data() {
    return SocketOptions();
}

std::ostream& operator<<(std::ostream& s, const SocketOptions& o) {
    o.print(s);
    return s;
}

}  // namespace net
}  // namespace eckit
