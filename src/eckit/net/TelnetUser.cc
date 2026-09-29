// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/net/TelnetUser.h"
#include "eckit/net/Telnet.h"
#include "eckit/runtime/Monitor.h"

namespace eckit::net {

TelnetUser::TelnetUser(TCPSocket& protocol) : NetUser(protocol), from_(protocol_.remoteHost()) {}

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

}  // namespace eckit::net
