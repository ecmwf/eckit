/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/net/TelnetUser.h"
#include "eckit/net/Telnet.h"
#include "eckit/runtime/Monitor.h"


namespace eckit {
namespace net {

TelnetUser::TelnetUser(TCPSocket& protocol) :
    NetUser(protocol), from_(protocol_.remoteHost()) {}

TelnetUser::~TelnetUser() {}

void TelnetUser::serve(Stream&, std::istream& in, std::ostream& out) {

    Log::debug() << "Starting a telnet connection " << std::endl;

    Monitor::instance().kind("telnet");
    Monitor::instance().name(from_);

    while (!stopped()) {
        out << "Telnet not supported any more (for now anyway)" << std::endl;
        break;
    }
    Log::info() << "Exiting telnet user loop..." << std::endl;
}

}  // namespace net
}  // namespace eckit
