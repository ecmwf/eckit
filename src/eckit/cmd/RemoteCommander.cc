/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/cmd/RemoteCommander.h"
#include "eckit/cmd/RemoteCommandUser.h"
#include "eckit/config/Resource.h"
#include "eckit/runtime/Monitor.h"


namespace eckit {


RemoteCommander::RemoteCommander(int p) :
    net::NetService(p) {
    Monitor::instance().port(port());
    Monitor::instance().host("localhost");
}


RemoteCommander::~RemoteCommander() {}


net::NetUser* RemoteCommander::newUser(net::TCPSocket& protocol) const {
    return new RemoteCommandUser(protocol);
}


}  // namespace eckit
