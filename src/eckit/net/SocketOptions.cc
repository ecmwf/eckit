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

#include "eckit/config/Resource.h"
#include "eckit/net/SocketOptions.h"

namespace eckit {
namespace net {

static void init(SocketOptions& opts) {

    static std::string bindAddr = Resource<std::string>("localBindingAddress", ""); /* "127.0.0.1" */

    opts.bindAddress(bindAddr);

    static int ListenBacklog = eckit::Resource<int>("socketOptionsListenBacklog", 5);

    opts.listenBacklog(ListenBacklog);

    static bool reusePort  = eckit::Resource<bool>("socketOptionsReusePort", false);
    static bool reuseAddr  = eckit::Resource<bool>("socketOptionsReuseAddr", false);
    static bool noLinger   = eckit::Resource<bool>("socketOptionsNoLinger", false);
    static bool keepAlive  = eckit::Resource<bool>("socketOptionsKeepAlive", true);
    static bool ipLowDelay = eckit::Resource<bool>("socketOptionsIpLowDelay", true);
    static bool tcpNoDelay = eckit::Resource<bool>("socketOptionsTcpNoDelay", true);

    static int receiveBufferSize = eckit::Resource<int>("socketOptionsReceiveBufferSize", 0);
    static int sendBufferSize = eckit::Resource<int>("socketOptionsSendBufferSize", 0);

    opts.reusePort(reusePort);
    opts.reuseAddr(reuseAddr);
    opts.noLinger(noLinger);
    opts.keepAlive(keepAlive);
    opts.ipLowDelay(ipLowDelay);
    opts.tcpNoDelay(tcpNoDelay);
    opts.receiveBufferSize(receiveBufferSize);
    opts.sendBufferSize(sendBufferSize);
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
      << "tcpNoDelay=" << tcpNoDelay_ << ", "
      << "receiveBufferSize=" << receiveBufferSize_ << ", "
      << "sendBufferSize=" << sendBufferSize_ 
      << "]" << std::endl;
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
