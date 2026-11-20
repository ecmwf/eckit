// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/net/Socket.h"

#include "eckit/exception/Exceptions.h"

namespace eckit::net {

//----------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Socket> Socket::acceptSocket(const std::string&, int) {
    NOTIMP;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::net
