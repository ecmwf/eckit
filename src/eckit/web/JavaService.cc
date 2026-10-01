// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/web/JavaService.h"
#include "eckit/web/JavaUser.h"


namespace eckit {


JavaService::JavaService(int port) : net::NetService(port) {}

JavaService::~JavaService() {}

net::NetUser* JavaService::newUser(net::TCPSocket& socket) const {
    return new JavaUser(socket);
}


}  // namespace eckit
