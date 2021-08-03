/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/net/Telnet.h"
#include "eckit/net/TelnetUser.h"
#include "eckit/runtime/Monitor.h"


namespace eckit {
namespace net {

Telnet::Telnet(int port) :
    NetService(port) {}

Telnet::~Telnet() {}


net::NetUser* Telnet::newUser(net::TCPSocket& protocol) const {
    return new TelnetUser(protocol);
}


}  // namespace net
}  // namespace eckit
