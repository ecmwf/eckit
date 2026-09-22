// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/net/Port.h"

#include "eckit/config/Resource.h"

namespace eckit::net {

Port::Port(const std::string& name, int port) : port_(port) {
    int offset = Resource<int>("portOffset", 0);
    port_      = Resource<int>(name + "Port", port) + offset;
}

}  // namespace eckit::net
