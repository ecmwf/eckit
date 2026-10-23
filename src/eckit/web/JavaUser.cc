// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/web/JavaUser.h"

#include <memory>
#include <stdexcept>

#include "eckit/serialisation/Stream.h"
#include "eckit/web/JavaAgent.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


JavaUser::JavaUser(net::TCPSocket& socket) : net::NetUser(socket) {}

JavaUser::~JavaUser() {}

void JavaUser::serve(eckit::Stream& s, std::istream& in, std::ostream& out) {
    JavaAgent::serve(s, in, out);
}

void JavaUser::refuse(eckit::Stream& s, std::istream&, std::ostream&, const std::string& reason) {
    // Consume the agent, then send the error where the client expects the clearance
    std::unique_ptr<JavaAgent> a(Reanimator<JavaAgent>::reanimate(s));
    s << std::runtime_error(reason);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
