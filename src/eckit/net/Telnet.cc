// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/net/Telnet.h"
#include "eckit/net/TelnetUser.h"
#include "eckit/runtime/Monitor.h"

namespace eckit::net {

Telnet::Telnet(int port) : NetService(port) {}

Telnet::~Telnet() {}


net::NetUser* Telnet::newUser(net::TCPSocket& protocol) const {
    return new TelnetUser(protocol);
}

}  // namespace eckit::net
