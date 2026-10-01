// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/cmd/RemoteCommander.h"
#include "eckit/cmd/RemoteCommandUser.h"
#include "eckit/config/Resource.h"
#include "eckit/runtime/Monitor.h"


namespace eckit {


RemoteCommander::RemoteCommander(int p) : net::NetService(p) {
    Monitor::instance().port(port());
    Monitor::instance().host("localhost");
}


RemoteCommander::~RemoteCommander() {}


net::NetUser* RemoteCommander::newUser(net::TCPSocket& protocol) const {
    return new RemoteCommandUser(protocol);
}


}  // namespace eckit
