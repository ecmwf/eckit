/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/net/Port.h"
#include "eckit/config/Resource.h"

namespace eckit::net {

Port::Port(const std::string& name, int port) : port_(port) {
    int offset = Resource<int>("portOffset", 0);
    port_      = Resource<int>(name + "Port", port) + offset;
}

}  // namespace eckit::net
