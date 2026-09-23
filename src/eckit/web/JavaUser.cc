// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/web/JavaUser.h"

#include "eckit/web/JavaAgent.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


JavaUser::JavaUser(net::TCPSocket& socket) : net::NetUser(socket) {}

JavaUser::~JavaUser() {}

void JavaUser::serve(eckit::Stream& s, std::istream& in, std::ostream& out) {
    JavaAgent::serve(s, in, out);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
