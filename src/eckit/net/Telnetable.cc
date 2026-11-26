// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/net/Telnetable.h"
#include "eckit/net/Telnet.h"

namespace eckit::net {

Telnetable::Telnetable(int port) : telnet_(new Telnet(port)) {
    if (port != 0) {
        telnet_.start();
    }
}

Telnetable::~Telnetable() {
    telnet_.stop();
}

}  // namespace eckit::net
