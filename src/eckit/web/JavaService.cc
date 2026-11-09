// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/web/JavaService.h"
#include "eckit/web/JavaUser.h"


namespace eckit {


JavaService::JavaService(int port) : net::NetService(port), preferProcess_(false) {}

JavaService::JavaService(int port, ConcurrencyLimiter& limiter, bool preferProcess) :
    net::NetService(port, limiter), preferProcess_(preferProcess) {}

JavaService::~JavaService() {}

net::NetUser* JavaService::newUser(net::TCPSocket& socket) const {
    return new JavaUser(socket);
}


}  // namespace eckit
